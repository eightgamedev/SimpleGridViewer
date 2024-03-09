# include <Siv3D.hpp> // Siv3D v0.6.14
# include "SpreadSheet.hpp"

void Main()
{
	Window::Resize(1920, 1009);
	Scene::SetBackground(Color(34, 116, 71));

	size_t columnCount = 20;
	size_t rowCount = 24;

	SpreadSheet::SpreadSheetGUI spreadSheetGUI{ {columnCount, rowCount }, { 10, 10 }, { 0, 0 } };

	Grid<String> values(columnCount, rowCount);
	Array<String> columnNames = SpreadSheet::AlphabetUtility::GenerateAlphabetArray(columnCount);

	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t column = 0; column < columnCount; ++column)
		{
			values[row][column] = U"{}{}"_fmt(columnNames[column], row);
		}
	}

	spreadSheetGUI.setValues(values);
	spreadSheetGUI.setTextFont(Font(15));


	while (System::Update())
	{
		const Transformer2D t{ Mat3x2::Translate(50, 50), TransformCursor::Yes };
		spreadSheetGUI.update();

		spreadSheetGUI.draw();
	}
}
