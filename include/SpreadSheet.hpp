# pragma once
# include "gridcell/CellGrid.hpp"
# include "SasaGUI/SasaGUI.hpp"

namespace SpreadSheet
{
	namespace Config
	{
		struct SheetHeader
		{		
			inline constexpr static int32 Height = 20;
			inline constexpr static ColorF BackgroundColor = Palette::Lightgray;
			inline constexpr static ColorF TextColor = Palette::Black;
		};

		struct SheetRow
		{
			inline constexpr static int32 Width = 60;
			inline constexpr static ColorF BackgroundColor = Palette::Lightgray;
			inline constexpr static ColorF TextColor = Palette::Black;
		};

		struct Cell
		{
			inline constexpr static int32 Height = 20;
			inline constexpr static int32 Width = 80;
			inline constexpr static ColorF BackgroundColor = Palette::White;
			inline constexpr static ColorF HoverColor{ 0.9, 0.9, 0.9, 0.5 };
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

	class SpreadSheetGUI
	{
		public:
			SpreadSheetGUI(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint);
			SpreadSheetGUI(const Size& sheetSize, const Size& visibleCellSize, const Point& viewPoint, const Array<String>& rowNames, const Array<String>& columnNames);
			void setValues(const Grid<String>& values);
			String getValue(int32 row, int32 column) const;
			void setTextFont(const Font& font);
			void update();
			void draw() const;
		private:
			void updateScrollBar();
			SizeF getSize() const noexcept;
			size_t getVisibleRowCount() const;
			size_t getVisibleColumnCount() const;
			bool isCellVisible(size_t row, size_t column) const;
			void drawSheetHeader() const;
			void drawSheetRows() const;
			void drawCells() const;
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
	};
}
