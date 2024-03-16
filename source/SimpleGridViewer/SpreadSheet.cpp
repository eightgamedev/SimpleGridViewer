# include "SimpleGridViewer/SpreadSheet.hpp"

namespace SimpleGridViewer
{
	String AlphabetUtility::ToAlphabet(size_t index)
	{
		if (index < 0)
		{
			return U"";
		}
		else if (index < 26)
		{
			return String(1, U'A' + index);
		}
		else
		{
			return ToAlphabet(index / 26 - 1) + ToAlphabet(index % 26);
		}
	}

	Array<String> AlphabetUtility::GenerateAlphabetArray(size_t index)
	{
		static Array<String> cache;
		for (size_t i = cache.size(); i <= index; ++i)
		{
			cache.push_back(ToAlphabet(i));
		}
		return Array<String>(cache.begin(), cache.begin() + index + 1);
	}

	void SpreadSheet::initialize(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint)
	{
		const double sheetWidth = visibleCellSize.x * Config::Cell::Width;
		const double sheetHeight = visibleCellSize.y * Config::Cell::Height;

		m_sheetArea = RectF{ viewPoint.x, viewPoint.y, sheetWidth + Config::SheetRow::Width, sheetHeight + Config::SheetHeader::Height };
		m_viewArea = RectF{ m_sheetArea.tl(), m_sheetArea.size + Size{SasaGUI::ScrollBar::Thickness, SasaGUI::ScrollBar::Thickness} };
		m_columnWidths = Array<int32>(sheetSize.x, Config::Cell::Width);
		m_rowHeights = Array<int32>(sheetSize.y, Config::Cell::Height);
		m_cellGrid = CellGrid(m_columnWidths, m_rowHeights);
		m_values = Grid<String>(sheetSize);
		m_lastVisibleRow = getVisibleRowCount() - 1;
		m_lastVisibleColumn = getVisibleColumnCount() - 1;
	}

	constexpr size_t DEFAULT_FONT_SIZE = 20;

	SpreadSheet::SpreadSheet(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint)
		: m_indexFont(DEFAULT_FONT_SIZE)
		, m_textFont(DEFAULT_FONT_SIZE)
	{
		initialize(sheetSize, visibleCellSize, viewPoint);

		for (size_t i = 0; i < sheetSize.x; ++i)
		{
			m_columnNames.push_back(Format(i));
		}
		for (size_t i = 0; i < sheetSize.y; ++i)
		{
			m_rowNames.push_back(Format(i));
		}
	}

	void SpreadSheet::setValues(const Grid<String>& values)
	{
		const size_t rowCount = Min(values.height(), m_values.height());
		const size_t columnCount = Min(values.width(), m_values.width());

		for (size_t row = 0; row < rowCount; ++row)
		{
			for (size_t column = 0; column < columnCount; ++column)
			{
				m_values[row][column] = values[row][column];
			}
		}
	}

	Optional<String> SpreadSheet::getValue(size_t row, size_t column) const
	{
		if (row < 0 || row >= m_values.height() || column < 0 || column >= m_values.width())
		{
			return none;
		}
		return m_values[row][column];
	}

	void SpreadSheet::setTextFont(const Font& font)
	{
		m_textFont = font;
	}

	void SpreadSheet::setRowNames(const Array<String>& rowNames)
	{
		m_rowNames = rowNames;
	}

	void SpreadSheet::setColumnNames(const Array<String>& columnNames)
	{
		m_columnNames = columnNames;
	}

	Optional<Point> SpreadSheet::getHoveredCell() const noexcept
	{
		return m_hoveredCell;
	}

	Optional<Point> SpreadSheet::getSelectedCell() const noexcept
	{
		return m_selectedCell;
	}

	void SpreadSheet::update()
	{
		{
			const Transformer2D verticalScrollBarMat{ Mat3x2::Translate(m_sheetArea.tr()), TransformCursor::Yes };
			if (m_verticalScrollBar.getThumbRect().mouseOver())
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}
		{
			const Transformer2D horizontalScrollBarMat{ Mat3x2::Translate(m_sheetArea.bl()), TransformCursor::Yes };
			if (m_horizontalScrollBar.getThumbRect().mouseOver())
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}
		if (m_viewArea.mouseOver())
		{
			updateScrollBar();
		}

		updateVisibleRows();
		updateVisibleColumns();

		{
			const Transformer2D sheetHeaderMat{ Mat3x2::Translate(m_sheetArea.x, m_sheetArea.y), TransformCursor::Yes };
			{
				const Transformer2D sheetRowsMat{ Mat3x2::Translate(0, Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
				updateSelectedRow();
			}
			{
				const Transformer2D t{ Mat3x2::Translate(
									Config::SheetRow::Width
									- (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), 0),
									TransformCursor::Yes };
				updateSelectedColumn();
			}
		}

		{
			const Transformer2D cellsMat{ Mat3x2::Translate(Config::SheetRow::Width - (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
			updateCells();
		}
	}

	void SpreadSheet::draw() const
	{
		{
			const Transformer2D sheetHeaderMat{ Mat3x2::Translate(m_sheetArea.x, m_sheetArea.y), TransformCursor::Yes };
			Rect{ 0, 0, Config::SheetRow::Width, Config::SheetHeader::Height }.draw(Config::SheetHeader::BackgroundColor);


			{
				const Transformer2D cellsMat{ Mat3x2::Translate(Config::SheetRow::Width - (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
				drawCells();
				drawGridLines();
			}

			{
				const Transformer2D t{ Mat3x2::Translate(
					Config::SheetRow::Width
					- (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), 0),
					TransformCursor::Yes };


				drawSheetHeader();
				drawSelectedColumn();
			}
			{
				const Transformer2D sheetRowsMat{ Mat3x2::Translate(0, Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
				drawSheetRows();
				drawSelectedRow();
			}


		}
		{
			const Transformer2D verticalScrollBarMat{ Mat3x2::Translate(m_sheetArea.tr()), TransformCursor::Yes };
			m_verticalScrollBar.draw();
		}

		{
			const Transformer2D horizontalScrollBarMat{ Mat3x2::Translate(m_sheetArea.bl()), TransformCursor::Yes };
			m_horizontalScrollBar.draw();
		}
	}

	void SpreadSheet::updateScrollBar()
	{
		{
			const Transformer2D t{ Mat3x2::Translate(m_sheetArea.tr()), TransformCursor::Yes };
			m_verticalScrollBar.updateLayout({
			0,
			0,
			SasaGUI::ScrollBar::Thickness,
			(int32)m_sheetArea.h
			});
			m_verticalScrollBar.updateConstraints(0.0, m_cellGrid.getTotalHeight(), m_sheetArea.h - Config::SheetHeader::Height);
			m_verticalScrollBar.scroll(Mouse::Wheel() * 30);
			m_verticalScrollBar.update();
		}

		{
			const Transformer2D t{ Mat3x2::Translate(m_sheetArea.bl()), TransformCursor::Yes };
			m_horizontalScrollBar.updateLayout({
			0,
			0,
			(int32)m_sheetArea.w,
			SasaGUI::ScrollBar::Thickness
			});
			m_horizontalScrollBar.updateConstraints(0.0, m_cellGrid.getTotalWidth(), m_sheetArea.w - Config::SheetRow::Width);
			m_horizontalScrollBar.update();
		}
	}

	SizeF SpreadSheet::getAreaSize() const noexcept
	{
		return m_sheetArea.size;
	}

	void SpreadSheet::updateVisibleColumns()
	{
		m_firstVisibleColumn = Min(m_cellGrid.getColumnCount() - 1, static_cast<size_t>(round(m_horizontalScrollBar.value() / Config::Cell::Width)));
		m_lastVisibleColumn = Min(m_cellGrid.getColumnCount() - 1, m_firstVisibleColumn + getVisibleColumnCount() - 1);
	}

	void SpreadSheet::updateVisibleRows()
	{
		m_firstVisibleRow = Min(m_cellGrid.getRowCount() - 1, static_cast<size_t>(floor(m_verticalScrollBar.value() / Config::Cell::Height)));
		m_lastVisibleRow = Min(m_cellGrid.getRowCount() - 1, m_firstVisibleRow + getVisibleRowCount() - 1);
	}

	void SpreadSheet::updateCells()
	{
		m_hoveredCell = m_cellGrid.getCellIndex(Cursor::Pos());
		if (m_hoveredCell.has_value() && isCellVisible(m_hoveredCell->y, m_hoveredCell->x) && MouseL.down())
		{
			m_selectedCell = m_hoveredCell;
			m_selectedRow = none;
			m_selectedColumn = none;
		}
	}

	void SpreadSheet::updateSelectedRow()
	{
		m_hoveredRow = m_cellGrid.getRowIndex(Cursor::Pos().y);
		if (not m_hoveredRow.has_value()) return;

		const size_t hoveredRow = m_hoveredRow.value();
		if (hoveredRow < m_cellGrid.getRowCount()
			&& m_firstVisibleRow <= hoveredRow && hoveredRow <= m_lastVisibleRow)
		{
			Rect rect = Rect{ 0, m_cellGrid.getCellY(hoveredRow), Config::SheetRow::Width, m_rowHeights[hoveredRow] };
			if (rect.leftClicked())
			{
				m_selectedRow = m_hoveredRow;
				m_selectedCell = none;
			}
		}
	}

	void SpreadSheet::updateSelectedColumn()
	{
		m_hoveredColumn = m_cellGrid.getColumnIndex(Cursor::Pos().x);
		if (not m_hoveredColumn.has_value()) return;

		const size_t hoveredColumn = m_hoveredColumn.value();
		if (hoveredColumn < m_cellGrid.getColumnCount()
						&& m_firstVisibleColumn <= hoveredColumn && hoveredColumn <= m_lastVisibleColumn)
		{
			Rect rect = Rect{ m_cellGrid.getCellX(hoveredColumn), 0, m_columnWidths[hoveredColumn], Config::SheetHeader::Height };
			if (rect.leftClicked())
			{
				m_selectedColumn = m_hoveredColumn;
				m_selectedCell = none;
			}
		}
	}
	
	size_t SpreadSheet::getVisibleRowCount() const
	{
		size_t count = 0;
		for (size_t i = 0; i < m_cellGrid.getRowCount(); ++i)
		{
			if (m_sheetArea.h < m_cellGrid.getCellY(i) + m_rowHeights[i] + Config::SheetHeader::Height)
			{
				break;
			}
			++count;
		}
		return count;
	}

	size_t SpreadSheet::getVisibleColumnCount() const
	{
		size_t count = 0;
		for (size_t i = 0; i < m_cellGrid.getColumnCount(); ++i)
		{
			if (m_sheetArea.w < m_cellGrid.getCellX(i) + m_columnWidths[i] + Config::SheetRow::Width)
			{
				break;
			}
			++count;
		}
		return count;
	}

	bool SpreadSheet::isCellVisible(size_t row, size_t column) const
	{
		if (column < m_firstVisibleColumn
		 || column > m_lastVisibleColumn
		 || row < m_firstVisibleRow
		 || row > m_lastVisibleRow)
		{
			return false;
		}
		return true;
	}

	void SpreadSheet::drawSheetHeader() const
	{
		for (size_t column = m_firstVisibleColumn; column <= m_lastVisibleColumn; ++column)
		{
			Rect rect = Rect{ m_cellGrid.getCellX(column), 0, m_columnWidths[column], Config::SheetHeader::Height};
			if ((rect.x + rect.w) < m_horizontalScrollBar.value()
			  || rect.x > (m_horizontalScrollBar.value() + m_sheetArea.w))
			{
				continue;
			}
			rect.draw(Config::SheetHeader::BackgroundColor);
			const String columnName = m_columnNames[column];
			m_indexFont(columnName).drawAt(rect.center(), Config::SheetHeader::TextColor);
		}

		for (size_t column = m_firstVisibleColumn; column <= m_lastVisibleColumn; ++column)
		{
			const int32 x = m_cellGrid.getCellX(column);
			if ((x + Config::SheetRow::Width) < m_horizontalScrollBar.value()
			  || x > (m_horizontalScrollBar.value() + m_sheetArea.w))
			{
				continue;
			}
			Rect{ x, 0, 1, Config::SheetHeader::Height }.draw(Config::Grid::Color);
		}
	}

	void SpreadSheet::drawSheetRows() const
	{
		for (size_t row = m_firstVisibleRow; row <= m_lastVisibleRow; ++row)
		{
			Rect rect = Rect{ 0, m_cellGrid.getCellY(row), Config::SheetRow::Width, m_rowHeights[row] };

			if ((rect.y + rect.h) < m_verticalScrollBar.value()
			  || rect.y > (m_verticalScrollBar.value() + m_sheetArea.h))
			{
				continue;
			}

			rect.draw(Config::SheetRow::BackgroundColor);
			const String rowName = m_rowNames[row];
			m_indexFont(rowName).drawAt(rect.center(), Config::SheetRow::TextColor);
		}

		for (size_t row = m_firstVisibleRow; row <= m_lastVisibleRow; ++row)
		{
			const int32 y = m_cellGrid.getCellY(row);
			if ((y + Config::SheetHeader::Height) < m_verticalScrollBar.value()
			  || y > (m_verticalScrollBar.value() + m_sheetArea.h))
			{
				continue;
			}
			Rect{ 0, y,  Config::SheetRow::Width, 1 }.draw(Config::Grid::Color);
		}
	}

	void SpreadSheet::drawCells() const
	{
		for (size_t row = m_firstVisibleRow; row <= m_lastVisibleRow; ++row)
		{
			for (size_t column = m_firstVisibleColumn; column <= m_lastVisibleColumn; ++column)
			{
				if (!isCellVisible(row, column))
				{
					continue;
				}
				const Rect rect = m_cellGrid.getCellRect(column, row);
				rect.draw(Config::Cell::BackgroundColor);
				m_textFont(m_values[row][column]).drawAt(m_cellGrid.getCellRect(column, row).center(), Config::Cell::TextColor);
			}
		}

		if (m_hoveredCell.has_value() && isCellVisible(m_hoveredCell->y, m_hoveredCell->x))
		{
			const Rect rect = m_cellGrid.getCellRect(m_hoveredCell->x, m_hoveredCell->y);
			rect.stretched(-1, 0, 0, -1).draw(Config::Cell::HoveredColor);
		}
		
		if (m_selectedCell.has_value() && isCellVisible(m_selectedCell->y, m_selectedCell->x))
		{
			const Rect rect = m_cellGrid.getCellRect(m_selectedCell->x, m_selectedCell->y);
			rect.stretched(-1, 0, 0, -1).drawFrame(1, 0, Config::Cell::SelectedColor);
		}
	}

	void SpreadSheet::drawSelectedRow() const
	{
		if (m_selectedRow.has_value() && m_selectedRow.value() < m_cellGrid.getRowCount())
		{
			const size_t row = m_selectedRow.value();
			const Rect rect = Rect{ 0, m_cellGrid.getCellY(row), m_sheetArea.asRect().w, m_rowHeights[row]};
			rect.drawFrame(1, 0, Config::SheetRow::SelectedColor);
		}
	}

	void SpreadSheet::drawSelectedColumn() const
	{
		if (m_selectedColumn.has_value() && m_selectedColumn.value() < m_cellGrid.getColumnCount())
		{
			size_t column = m_selectedColumn.value();
			const Rect rect = Rect{ m_cellGrid.getCellX(column), 0, m_columnWidths[column], m_sheetArea.asRect().h};
			rect.drawFrame(1, 0, Config::SheetHeader::SelectedColor);
		}
	}

	void SpreadSheet::drawGridLines() const
	{
		const int32 width = m_columnWidths[0];
		const int32 height = m_rowHeights[0];

		for (size_t column = m_firstVisibleColumn; column <= m_lastVisibleColumn; ++column)
		{
			const int32 x = m_cellGrid.getCellX(column);

			for (size_t row = m_firstVisibleRow; row <= m_lastVisibleRow; ++row)
			{
				const int32 y = m_cellGrid.getCellY(row);

				if ((x + width) < m_horizontalScrollBar.value() || (y + height) < m_verticalScrollBar.value() ||
					x > (m_horizontalScrollBar.value() + m_sheetArea.w) || y > (m_verticalScrollBar.value() + m_sheetArea.h))
				{
					continue;
				}
				RectF{ x, y, 1, height }.draw(Config::Grid::Color);
				RectF{ x, y, width, 1 }.draw(Config::Grid::Color);
			}
		}
	}
}


