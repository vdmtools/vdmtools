このフォルダーは、VDM++の関数型プログラミング用ライブラリーの記述用です。
なお、このフォルダー下のVDM++ファイルのクラス名は、SSLibと一緒に使えるよう、頭にFを付ける。
こうすることで、例えば、SSLibのSequenceクラスとFPLibのFSequenceクラスを両方同時に使うことができる。

ファイルの説明
*.tex				--モジュールのドキュメント兼VDM++清書ソースの最上位ソース
F*T.vpp				--各クラスの回帰テストケース
FBusinessTable.vpp	--ビジネス上の規則に使用される表に関わる関数を提供する
FCalendar.vpp		--暦に関わる関数を提供する
FCharacter.vpp		--文字に関わる関数を提供する
FHashtable.vpp		--ハッシュ表に関わる関数を提供する
FSequence.vpp		--列に関わる関数を提供する
FTestDriver.vpp		--回帰テスト支援ライブラリの本体。テストケースの実行に関わる関数を提供する
FTestLogger.vpp		--回帰テストのログを管理する関数を提供する
FFunction.vpp		--関数に関わる関数を提供する
FInteger.vpp			--整数に関わる関数を提供する
FJapaneseCalendar.vpp	--日本の暦に関わる関数を提供する
FMap.vpp			--写像に関わる関数を提供する
FNumber.vpp		--数に関わる共通関数を定義する
FProduct.vpp		--組に関わる関数を定義する
FQueue.vpp			--待ち行列に関する関数を定義する
FReal.vpp			--実数に関わる関数を提供する
FSet.vpp			--集合に関わる関数を提供する
FString.vpp			--文字列に関わる関数を提供する
