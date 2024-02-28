# pragma once
# include "gridcell/GuiGridAxis.hpp"

class CellGrid {
public:

	CellGrid() = default;

	/// @brief 各列の幅と各行の高さを指定して CellGrid を作成します。
	/// @param columnWidths 各列の幅（ピクセル）
	/// @param rowHeights 各行の高さ（ピクセル）
	CellGrid(const Array<int32>& columnWidths, const Array<int32>& rowHeights);

	/// @brief 列の個数を返します。
	/// @return 列の個数
	[[nodiscard]]
	size_t getColumnCount() const noexcept;

	/// @brief 行の個数を返します。
	/// @return 行の個数
	[[nodiscard]]
	size_t getRowCount() const noexcept;

	/// @brief セルの個数が 0 であるかを返します。
	/// @return セルの個数が 0 である場合 true, それ以外の場合は false
	[[nodiscard]]
	bool isEmpty() const noexcept;

	/// @brief セルの個数を返します。
	/// @return セルの個数
	[[nodiscard]]
	size_t getNumCells() const noexcept;

	/// @brief 行と列の情報をクリアします。
	void clear() noexcept;

	/// @brief 各列の幅を返します。
	/// @return 各列の幅（ピクセル）
	[[nodiscard]]
	const Array<int32>& getColumnWidths() const noexcept;

	/// @brief 各行の高さを返します。
	/// @return 各行の高さ（ピクセル）
	[[nodiscard]]
	const Array<int32>& getRowHeights() const noexcept;

	/// @brief 列の幅を追加します。
	/// @param width 列の幅（ピクセル）
	/// @remark この関数を呼び出すと、列の個数が 1 増えます。
	void addColumn(int32 width);

	/// @brief 行の高さを追加します。
	/// @param height 行の高さ（ピクセル）
	/// @remark この関数を呼び出すと、行の個数が 1 増えます。
	void addRow(int32 height);

	/// @brief 指定した列を削除します。
	/// @param column 列
	void removeColumn(size_t column);

	/// @brief 指定した行を削除します。
	/// @param row 行
	void removeRow(size_t row);

	/// @brief 列を挿入します。
	/// @param column 挿入する列の位置
	/// @param width 列の幅（ピクセル）
	void insertColumn(size_t column, int32 width);

	/// @brief 行を挿入します。
	/// @param row 挿入する行の位置
	/// @param height 行の高さ（ピクセル）
	void insertRow(size_t row, int32 height);

	/// @brief 指定した列の幅を変更します。
	/// @param column 列
	/// @param width 列の幅（ピクセル）
	void setColumnWidth(size_t column, int32 width) noexcept;

	/// @brief 指定した行の高さを変更します。
	/// @param row 行
	/// @param height 行の高さ（ピクセル）
	void setRowHeight(size_t row, int32 height) noexcept;

	/// @brief 指定した列の幅を返します。
	/// @param column 列
	/// @return 列の幅（ピクセル）
	[[nodiscard]]
	int32 getColumnWidth(size_t column) const noexcept;

	/// @brief 指定した行の高さを返します。
	/// @param row 行
	/// @return 行の高さ（ピクセル）
	[[nodiscard]]
	int32 getRowHeight(size_t row) const noexcept;

	/// @brief 指定したインデックスのセルのサイズを返します。
	/// @param column 列
	/// @param row 行
	/// @return セルのサイズ（ピクセル）
	Size getCellSize(size_t column, size_t row) const noexcept;

	/// @brief 全ての列の幅の合計を返します。
	/// @return 全ての列の幅の合計（ピクセル）
	[[nodiscard]]
	int32 getTotalWidth() const noexcept;

	/// @brief 全ての行の高さの合計を返します。
	/// @return 全ての行の高さの合計（ピクセル）
	[[nodiscard]]
	int32 getTotalHeight() const noexcept;

	/// @brief 指定した列が始まる X 座標を返します。
	/// @param column 列
	/// @return 指定した列が始まる X 座標（ピクセル）。列が存在しない場合は最後の列の終端の座標を返します。
	[[nodiscard]]
	int32 getCellX(size_t column) const noexcept;

	/// @brief 指定した行が始まる Y 座標を返します。
	/// @param row 行
	/// @return 指定した行が始まる Y 座標（ピクセル）。行が存在しない場合は最後の行の終端の座標を返します。
	[[nodiscard]]
	int32 getCellY(size_t row) const noexcept;

	/// @brief 指定したインデックスのセルの位置を返します。
	/// @param column 列
	/// @param row 行
	/// @return セルの位置（ピクセル）
	/// @remark セルの位置は、セルの左上隅の座標です。
	[[nodiscard]]
	Point getCellPosition(size_t column, size_t row) const noexcept;

	/// @brief 指定したインデックスのセルの範囲を返します。
	/// @param column 列
	/// @param row 行
	/// @return セルの範囲（ピクセル）
	/// @remark セルの範囲は、セルの左上隅の座標とセルのサイズからなります。
	[[nodiscard]]
	Rect getCellRect(size_t column, size_t row) const noexcept;

	/// @brief 指定した X 座標がどの列に属するかを返します。
	/// @param x X 座標
	/// @return 列のインデックス。指定した座標がどの列にも属さない場合は、none を返します。
	[[nodiscard]]
	Optional<size_t> getColumnIndex(int32 x) const noexcept;

	/// @brief 指定した Y 座標がどの行に属するかを返します。
	/// @param y Y 座標
	/// @return 行のインデックス。指定した座標がどの行にも属さない場合は、none を返します。
	[[nodiscard]]
	Optional<size_t> getRowIndex(int32 y) const noexcept;

	/// @brief 指定した座標がどのセルに属するかを返します。
	/// @param pos 座標
	/// @return セルのインデックス。指定した座標がどのセルにも属さない場合は、none を返します。
	[[nodiscard]]
	Optional<Point> getCellIndex(Point pos) const noexcept;

private:

	// 各列の幅（ピクセル）
	GuiGridAxis m_columnWidths;

	// 各行の高さ（ピクセル）
	GuiGridAxis m_rowHeights;
};
