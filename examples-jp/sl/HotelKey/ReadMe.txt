--VDM-SL仕様ファイル作成上の注意点
（１）VDM-SL仕様ファイル名は.vdmで終わること
（２）タブ（TAB）を使わない
ASCII形式の清書用PDFファイルを生成するため、VDM-SLのソースファイル内では、TABコードを使わず、空白で段付けしたほうがよい。
TABコードを使うと、段付けの幅が大きくなり、行の幅が大きくなり行の折り返しが発生しやすくなる。

--VDM-SLの回帰テスト実行とPDFファイル作成
shellコマンドからvdmdeコマンドを実行するだけでよい。
.vdmdeスクリプトが自動的に実行され、仕様の構文チェック・型チェック・回帰テストを実行し、ascii形式とTeX形式両方のPDFファイルを作成する。
bibliographyやrtinfoに関するエラーメッセージが出るが、returnキーを押して続行してよい。


--ファイルとディレクトリの説明

*.aux, *.dvi, *.idx, *.log, *.toc	-- TeXの作業用ファイル
*.vpp.tex		-- VDMToolsが生成する清書用TeXファイル
*.opt, *.wgm		-- VDMToolsで使用するファイル	

.vdmde			-- vdmdeの初期実行スクリプト（.vdmde-asciiと.vdmde-texを実行する）
.vdmde-ascii		-- vdmdeのスクリプト（構文チェックから、回帰テスト、清書用ファイル生成、HotelKey-ascii.pdfファイル作成まで行う）
.vdmde-tex		-- vdmdeのスクリプト（構文チェックから、回帰テスト、清書用ファイル生成、HotelKey.pdfファイル作成まで行う）
./EUC			-- EUCコードに変換したファイルを格納する作業用ディレクトリ(EUCコードでないと、perlが日本語を正しく変換できないため）
Abstract.tex		-- 概要を記述したTeXファイル
Hotel.prj		-- VDMToolsのプロジェクトファイル
Hotel.vdm		-- ホテルの鍵に関するVDM-SL仕様
HotelKey-ascii.pdf	-- ASCII形式の清書用PDFファイル
HotelKey-ascii.tex	-- HotelKey-ascii.pdfの元となるTeXファイル
HotelKey.pdf		-- TeX形式VDM++ソースとコードカバレージ情報を含むPDFファイル
HotelKey.tex		-- HotelKey.pdfの元となるTeXファイル
HotelT.vdm		-- 回帰テストのテストケースを定義したVDM++ファイル
Makefile		-- TeX形式の清書用PDFファイルを生成するMakefile
Makefile-ascii	-- ASCIIの清書用PDFファイルを生成するMakefile
TestDriver.vdm	-- 回帰テスト用ライブラリ
TestLogger.vdm	-- 回帰テスト用ログ作成
VDMTESTLOG.TXT	-- 回帰テストライブラリの作成するテスト結果のログ・ファイル
io.vdm			-- VDMToolsの標準IOライブラリ
replace.pl		-- VDMソースを含むTeXファイルを、普通のTeX形式に変換するPerlスクリプト
Title.tex		-- 肩書きなどを記述したTeXファイル
vdm.tc			-- VDMToolsが生成するコードカバレージ情報