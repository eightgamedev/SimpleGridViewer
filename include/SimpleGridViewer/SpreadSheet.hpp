# pragma once
# include "gridcell/CellGrid.hpp"
# include "SasaGUI/SasaGUI.hpp"

namespace SimpleGridViewer
{
	namespace Config
	{
		struct SheetHeader
		{		
			inline constexpr static int32 Height = 20;
			inline constexpr static ColorF BackgroundColor = Palette::Lightgray;
			inline constexpr static ColorF TextColor = Palette::Black;
			inline constexpr static ColorF HoveredColor{ 0.9, 0.9, 0.9, 0.5 };
			inline constexpr static ColorF SelectedColor{ 1.0, 0.0, 0.0, 1.0 };
		};

		struct SheetRow
		{
			inline constexpr static int32 Width = 60;
			inline constexpr static ColorF BackgroundColor = Palette::Lightgray;
			inline constexpr static ColorF TextColor = Palette::Black;
			inline constexpr static ColorF HoveredColor{ 0.9, 0.9, 0.9, 0.5 };
			inline constexpr static ColorF SelectedColor{ 1.0, 0.0, 0.0, 1.0 };
		};

		struct Cell
		{
			inline constexpr static int32 Height = 20;
			inline constexpr static int32 Width = 80;
			inline constexpr static ColorF BackgroundColor = Palette::White;
			inline constexpr static ColorF HoveredColor{ 0.9, 0.9, 0.9, 0.5 };
			inline constexpr static ColorF SelectedColor{ 1.0, 0.0, 0.0, 1.0 };
			inline constexpr static ColorF TextColor = Palette::Black;
		};

		struct Grid
		{
			inline constexpr static ColorF Color{ 0.11 };
		};
	};

	class AlphabetUtility
	{
	public:
		static String ToAlphabet(size_t index);
		static Array<String> GenerateAlphabetArray(size_t index);
	};

	class SpreadSheet
	{
		public:
			SpreadSheet(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint);
			void setValues(const Grid<String>& values);
			Optional<String> getValue(size_t row, size_t column) const;
			void setTextFont(const Font& font);
			void setRowNames(const Array<String>& rowNames);
			void setColumnNames(const Array<String>& columnNames);
			SizeF getAreaSize() const noexcept;
			Optional<Point> getHoveredCell() const noexcept;
			Optional<Point> getSelectedCell() const noexcept;
			void update();
			void draw() const;
		private:
			void initialize(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint);
			void updateScrollBar();
			void updateVisibleColumns();
			void updateVisibleRows();
			void updateCells();
			void updateSelectedRow();
			void updateSelectedColumn();
			size_t getVisibleRowCount() const;
			size_t getVisibleColumnCount() const;
			bool isCellVisible(size_t row, size_t column) const;
			void drawSheetHeader() const;
			void drawSheetRows() const;
			void drawCells() const;
			void drawSelectedRow() const;
			void drawSelectedColumn() const;
			void drawGridLines() const;
			Array<int32> m_rowHeights;
			Array<int32> m_columnWidths;
			Grid<String> m_values;
			RectF m_viewArea;
			RectF m_sheetArea;
			SasaGUI::ScrollBar m_verticalScrollBar{ SasaGUI::Orientation::Vertical };
			SasaGUI::ScrollBar m_horizontalScrollBar{ SasaGUI::Orientation::Horizontal };
			size_t m_firstVisibleRow = 0;
			size_t m_firstVisibleColumn = 0;
			size_t m_lastVisibleRow;
			size_t m_lastVisibleColumn;
			CellGrid m_cellGrid;
			Array<String> m_rowNames;
			Array<String> m_columnNames;
			Font m_indexFont;
			Font m_textFont;
			Optional<Point> m_hoveredCell;
			Optional<Point> m_selectedCell;
			Optional<size_t> m_hoveredRow;
			Optional<size_t> m_hoveredColumn;
			Optional<size_t> m_selectedRow;
			Optional<size_t> m_selectedColumn;
	};
}
