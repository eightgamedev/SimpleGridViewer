# include "gridcell/CellGrid.hpp"

/// @brief 各列の幅と各行の高さを指定して CellGrid を作成します。
/// @param columnWidths 各列の幅（ピクセル）
/// @param rowHeights 各行の高さ（ピクセル）
CellGrid::CellGrid(const Array<int32>& columnWidths, const Array<int32>& rowHeights)
	: m_columnWidths(columnWidths)
	, m_rowHeights(rowHeights) {}

/// @brief 列の個数を返します。
/// @return 列の個数
[[nodiscard]]
size_t CellGrid::getColumnCount() const noexcept
{
	return m_columnWidths.size();
}

/// @brief 行の個数を返します。
/// @return 行の個数
[[nodiscard]]
size_t CellGrid::getRowCount() const noexcept
{
	return m_rowHeights.size();
}

/// @brief セルの個数が 0 であるかを返します。
/// @return セルの個数が 0 である場合 true, それ以外の場合は false
[[nodiscard]]
bool CellGrid::isEmpty() const noexcept
{
	return (m_columnWidths.isEmpty() || m_rowHeights.isEmpty());
}

/// @brief セルの個数を返します。
/// @return セルの個数
[[nodiscard]]
size_t CellGrid::getNumCells() const noexcept
{
	return getRowCount() * getColumnCount();
}

/// @brief 行と列の情報をクリアします。
void CellGrid::clear() noexcept
{
	m_columnWidths.clear();
	m_rowHeights.clear();
}

/// @brief 各列の幅を返します。
/// @return 各列の幅（ピクセル）
[[nodiscard]]
const Array<int32>& CellGrid::getColumnWidths() const noexcept
{
	return m_columnWidths.getWidthArray();
}

/// @brief 各行の高さを返します。
/// @return 各行の高さ（ピクセル）
[[nodiscard]]
const Array<int32>& CellGrid::getRowHeights() const noexcept
{
	return m_rowHeights.getWidthArray();
}

/// @brief 列の幅を追加します。
/// @param width 列の幅（ピクセル）
/// @remark この関数を呼び出すと、列の個数が 1 増えます。
void CellGrid::addColumn(int32 width)
{
	m_columnWidths.insert(getColumnCount(), width);
}

/// @brief 行の高さを追加します。
/// @param height 行の高さ（ピクセル）
/// @remark この関数を呼び出すと、行の個数が 1 増えます。
void CellGrid::addRow(int32 height)
{
	m_rowHeights.insert(getRowCount(), height);
}

/// @brief 指定した列を削除します。
/// @param column 列
void CellGrid::removeColumn(size_t column)
{
	assert(column < m_columnWidths.size());
	m_columnWidths.erase(column);
}

/// @brief 指定した行を削除します。
/// @param row 行
void CellGrid::removeRow(size_t row)
{
	assert(row < m_rowHeights.size());
	m_rowHeights.erase(row);
}

/// @brief 列を挿入します。
/// @param column 挿入する列の位置
/// @param width 列の幅（ピクセル）
void CellGrid::insertColumn(size_t column, int32 width)
{
	assert(column <= m_columnWidths.size());
	m_columnWidths.insert(column, width);
}

/// @brief 行を挿入します。
/// @param row 挿入する行の位置
/// @param height 行の高さ（ピクセル）
void CellGrid::insertRow(size_t row, int32 height)
{
	assert(row <= m_rowHeights.size());
	m_rowHeights.insert(row, height);
}

/// @brief 指定した列の幅を変更します。
/// @param column 列
/// @param width 列の幅（ピクセル）
void CellGrid::setColumnWidth(size_t column, int32 width) noexcept
{
	assert(column < m_columnWidths.size());
	m_columnWidths.setWidth(column, width);
}

/// @brief 指定した行の高さを変更します。
/// @param row 行
/// @param height 行の高さ（ピクセル）
void CellGrid::setRowHeight(size_t row, int32 height) noexcept
{
	assert(row < m_rowHeights.size());
	m_rowHeights.setWidth(row, height);
}

/// @brief 指定した列の幅を返します。
/// @param column 列
/// @return 列の幅（ピクセル）
[[nodiscard]]
int32 CellGrid::getColumnWidth(size_t column) const noexcept
{
	assert(column < m_columnWidths.size());
	return m_columnWidths.getWidth(column);
}

/// @brief 指定した行の高さを返します。
/// @param row 行
/// @return 行の高さ（ピクセル）
[[nodiscard]]
int32 CellGrid::getRowHeight(size_t row) const noexcept
{
	assert(row < m_rowHeights.size());
	return m_rowHeights.getWidth(row);
}

/// @brief 指定したインデックスのセルのサイズを返します。
/// @param column 列
/// @param row 行
/// @return セルのサイズ（ピクセル）
Size CellGrid::getCellSize(size_t column, size_t row) const noexcept
{
	assert(column < m_columnWidths.size());
	assert(row < m_rowHeights.size());
	return{ m_columnWidths.getWidth(column), m_rowHeights.getWidth(row) };
}

/// @brief 全ての列の幅の合計を返します。
/// @return 全ての列の幅の合計（ピクセル）
[[nodiscard]]
int32 CellGrid::getTotalWidth() const noexcept
{
	return m_columnWidths.totalWidth();
}

/// @brief 全ての行の高さの合計を返します。
/// @return 全ての行の高さの合計（ピクセル）
[[nodiscard]]
int32 CellGrid::getTotalHeight() const noexcept
{
	return m_rowHeights.totalWidth();
}

/// @brief 指定した列が始まる X 座標を返します。
/// @param column 列
/// @return 指定した列が始まる X 座標（ピクセル）。列が存在しない場合は最後の列の終端の座標を返します。
[[nodiscard]]
int32 CellGrid::getCellX(size_t column) const noexcept
{
	column = Min(column, m_columnWidths.size());
	return m_columnWidths.indexToPos(column);
}

/// @brief 指定した行が始まる Y 座標を返します。
/// @param row 行
/// @return 指定した行が始まる Y 座標（ピクセル）。行が存在しない場合は最後の行の終端の座標を返します。
[[nodiscard]]
int32 CellGrid::getCellY(size_t row) const noexcept
{
	row = Min(row, m_rowHeights.size());
	return m_rowHeights.indexToPos(row);
}

/// @brief 指定したインデックスのセルの位置を返します。
/// @param column 列
/// @param row 行
/// @return セルの位置（ピクセル）
/// @remark セルの位置は、セルの左上隅の座標です。
[[nodiscard]]
Point CellGrid::getCellPosition(size_t column, size_t row) const noexcept
{
	return{ getCellX(column), getCellY(row) };
}

/// @brief 指定したインデックスのセルの範囲を返します。
/// @param column 列
/// @param row 行
/// @return セルの範囲（ピクセル）
/// @remark セルの範囲は、セルの左上隅の座標とセルのサイズからなります。
[[nodiscard]]
Rect CellGrid::getCellRect(size_t column, size_t row) const noexcept
{
	auto [x, w] = m_columnWidths.getCellRange(column);
	auto [y, h] = m_rowHeights.getCellRange(row);
	return Rect(x, y, w, h);
}

/// @brief 指定した X 座標がどの列に属するかを返します。
/// @param x X 座標
/// @return 列のインデックス。指定した座標がどの列にも属さない場合は、none を返します。
[[nodiscard]]
Optional<size_t> CellGrid::getColumnIndex(int32 x) const noexcept
{
	if (x < 0 || getTotalWidth() <= x) return none;
	return m_columnWidths.posToIndex(x);
}

/// @brief 指定した Y 座標がどの行に属するかを返します。
/// @param y Y 座標
/// @return 行のインデックス。指定した座標がどの行にも属さない場合は、none を返します。
[[nodiscard]]
Optional<size_t> CellGrid::getRowIndex(int32 y) const noexcept
{
	if (y < 0 || getTotalHeight() <= y) return none;
	return m_rowHeights.posToIndex(y);
}

/// @brief 指定した座標がどのセルに属するかを返します。
/// @param pos 座標
/// @return セルのインデックス。指定した座標がどのセルにも属さない場合は、none を返します。
[[nodiscard]]
Optional<Point> CellGrid::getCellIndex(Point pos) const noexcept
{
	const auto column = getColumnIndex(pos.x);
	const auto row = getRowIndex(pos.y);

	if (column && row)
	{
		return Point{ *column, *row };
	}
	else
	{
		return none;
	}
}
