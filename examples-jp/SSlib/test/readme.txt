$Id: readme.txt,v 1.1 2005/10/31 02:15:42 vdmtools Exp $
SSLib -- VDM++ライブラリのテストプログラム written by Shin Sahara

ファイルの説明
	*.vpp		--VDM++のソース（テキストファイル版）

	AllT.vpp		--単体テスト用プログラム（テキストファイル版）
	CalendarT.vpp	--暦クラスを検査する
	DateT.vpp		--日付クラスを検査する
	DoubleListQueueT.vpp	--２重リストで実装された待ち行列クラスを検査する
	HashtableT.vpp	--ハッシュ表を検査する
	IntegerT.vpp	--整数クラスを検査する
	MapT.vpp		--写像クラスを検査する
	ObjectT.vpp	--オブジェクトクラスを検査する
	RealT.vpp		--十巣クラスを検査する
	SequenceT.vpp	--列クラスを検査する
	SetT.vpp		--集合クラスを検査する
	StringT.vpp		--文字列クラスを検査する
	TestCase.vpp	--回帰テスト用テストケースのスーパークラス
	TestDriver.vpp	--回帰テスト用テストドライバー
	TestLogger.vpp	--回帰テスト用テスト結果出力
	UniqueSequenceT.vpp	-- 唯一の番号を発番するクラスを検査する
	vdm.tc		--テストカバレージの結果が入っているデータ
	VDMTESTLOG.TXT	--テスト結果のログ

単体テストの実行方法
	事前条件
		VDMToolsのインストールが終了していること
		インタープリタのPretty Printerオプション設定で、Output index of definitions and uses と Test coverage coloring をチェックしておくこと
		コードカバレージ情報を得たいときは、VDM++のソース（RTF版）を使用する。
			（VDM++のソース（テキストファイル版）を使用するとTeXファイルが生成されるが、今のところうまく表示される保証はない。）
	手順
		test.prjファイルをダブルクリック
		静的型チェックを行わなくてもインタープリタによる実行はできるが、静的型チェックをやりたければここで Type Check（メニューかボタンから）する
		インタープリタウィンドウでイニシャライズ
		インタープリタウィンドウで以下のコマンドを実行（--以下は注釈なので、実際にはタイプしない）
			cr a := new AllT()	--テスト用クラスのインスタンスを作成指定 a に代入
			d a.run()			--オブジェクト a の run 操作をデバッグ実行(デバッグが必要なければ p a.run() で実行することもできる）
			tcov write vdm.tc	--コードカバレージの結果をファイル vdm.tcに書き出す
			rtinfo vdm.tc		--コードカバレージ情報をインタープリタ上で表示する
		コードカバレージ情報を含んだ清書ファイルを得たいときは、Managerウィンドウで Pretty Print（メニューかボタンから）すると .rtf.rtf で終わるファイルが生成される。
			
			