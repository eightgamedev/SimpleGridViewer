﻿# include "SpreadSheet.hpp"

namespace SpreadSheet
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

	SpreadSheetGUI::SpreadSheetGUI(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint, const bool alphabetMode)
		: m_indexFont(20)
		, m_textFont(20)
		, m_alphabetMode(alphabetMode)
	{
		const double sheetWidth = visibleCellSize.x * Config::Cell::Width;
		const double sheetHeight = visibleCellSize.y * Config::Cell::Height;
		
		m_sheetArea = RectF{ viewPoint.x, viewPoint.y, sheetWidth + Config::SheetRow::Width, sheetHeight + Config::SheetHeader::Height };
		m_viewArea = RectF{ m_sheetArea.tl(), m_sheetArea.size + Size{SasaGUI::ScrollBar::Thickness, SasaGUI::ScrollBar::Thickness}};
		m_columnWidths = Array<int32>(sheetSize.x, Config::Cell::Width);
		m_rowHeights = Array<int32>(sheetSize.y, Config::Cell::Height);
		m_cellGrid = CellGrid(m_columnWidths, m_rowHeights);
		m_values = Grid<String>(sheetSize);
		ColumnNames = AlphabetUtility::GenerateAlphabetArray(sheetSize.x);
	}

	void SpreadSheetGUI::setValues(const Grid<String>& values)
	{
		m_values = values;
	}

	String SpreadSheetGUI::getValue(int32 row, int32 column) const
	{
		return m_values[row][column];
	}

	void SpreadSheetGUI::setTextFont(const Font& font)
	{
		m_textFont = font;
	}

	void SpreadSheetGUI::setAlphabetMode(bool alphabetMode)
	{
		m_alphabetMode = alphabetMode;
	}

	void SpreadSheetGUI::update()
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

		m_firstVisibleColumn = static_cast<size_t>(round(m_horizontalScrollBar.value() / Config::Cell::Width));
		m_firstVisibleColumn = Max(size_t(0), Min(m_cellGrid.getColumnCount(), m_firstVisibleColumn));

		m_firstVisibleRow = static_cast<size_t>(floor(m_verticalScrollBar.value() / Config::Cell::Height));
		m_firstVisibleRow = Max(size_t(0), Min(m_cellGrid.getRowCount(), m_firstVisibleRow));

		m_lastVisibleColumn = Min(m_cellGrid.getColumnCount(), m_firstVisibleColumn + getVisibleColumnCount() - 1);
		m_lastVisibleRow = Min(m_cellGrid.getRowCount(), m_firstVisibleRow + getVisibleRowCount() - 1);
	}

	void SpreadSheetGUI::draw() const
	{
		{
			const Transformer2D sheetHeaderMat{ Mat3x2::Translate(m_sheetArea.x, m_sheetArea.y), TransformCursor::Yes };
			Rect{ 0, 0, Config::SheetRow::Width, Config::SheetHeader::Height }.draw(Config::SheetHeader::BackgroundColor);
			{
				const Transformer2D t{ Mat3x2::Translate(
					Config::SheetRow::Width
					- (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), 0),
					TransformCursor::Yes };


				drawSheetHeader();
			}
			{
				const Transformer2D sheetRowsMat{ Mat3x2::Translate(0, Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
				drawSheetRows();
			}

			{
				const Transformer2D cellsMat{ Mat3x2::Translate(Config::SheetRow::Width - (static_cast<int>(round(m_horizontalScrollBar.value() / Config::Cell::Width) * Config::Cell::Width)), Config::SheetHeader::Height - (static_cast<int>(floor(m_verticalScrollBar.value() / Config::Cell::Height)) * Config::Cell::Height)), TransformCursor::Yes };
				drawCells();
				drawGridLines();
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

	void SpreadSheetGUI::updateScrollBar()
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

	SizeF SpreadSheetGUI::getSize() const noexcept
	{
		return m_sheetArea.size;
	}

	size_t SpreadSheetGUI::getVisibleRowCount() const
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

	size_t SpreadSheetGUI::getVisibleColumnCount() const
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

	bool SpreadSheetGUI::isCellVisible(size_t row, size_t column) const
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

	void SpreadSheetGUI::drawSheetHeader() const
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
			const String columnString = m_alphabetMode ? ColumnNames[column] : Format(column);
			m_indexFont(columnString).drawAt(rect.center(), Config::SheetHeader::TextColor);
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

	void SpreadSheetGUI::drawSheetRows() const
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
			const String rowString = Format(row);
			m_indexFont(rowString).drawAt(rect.center(), Config::SheetRow::TextColor);
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

	void SpreadSheetGUI::drawCells() const
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

		if (const Optional<Point> cell = m_cellGrid.getCellIndex(Cursor::Pos()))
		{
			if (!isCellVisible(cell->y, cell->x))
			{
				return;
			}
			const Rect rect = m_cellGrid.getCellRect(cell->x, cell->y);
			rect.stretched(-1, 0, 0, -1).draw(Config::Cell::HoverColor);
		}
	}

	void SpreadSheetGUI::drawGridLines() const
	{
		for (size_t column = m_firstVisibleColumn; column <= m_lastVisibleColumn; ++column)
		{
			const int32 x = m_cellGrid.getCellX(column);

			for (size_t row = m_firstVisibleRow; row <= m_lastVisibleRow; ++row)
			{
				const int32 y = m_cellGrid.getCellY(row);
				const int32 width = m_columnWidths[0];
				const int32 height = m_rowHeights[0];

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


