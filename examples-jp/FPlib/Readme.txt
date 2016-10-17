$Header: /home/vdmtools/cvsroot/toolbox/examples-jp/FPlib/Readme.txt,v 1.1 2005/10/31 02:09:58 vdmtools Exp $

このフォルダーは、VDM++の関数型プログラミング用ライブラリーの記述用です。
なお、このフォルダー下のVDM++ファイルのクラス名は、SSLibと一緒に使えるよう、頭にFを付ける。
こうすることで、例えば、SSLibのSequenceクラスとFPLibのFSequenceクラスを両方同時に使うことができる。
ファイル名は頭にＦを付けず、SSLibと同じ命名規則としている。

ファイルの説明
*.tex				--モジュールのドキュメント兼VDM++清書ソースの最上位ソース
BusinessTable.vpp	--ビジネス上の規則に使用される表に関わる関数を提供する
Character.vpp		--文字に関わる関数を提供する
Function.vpp		--関数に関わる関数を提供する
Hashtable.vpp		--ハッシュ表に関わる関数を提供する
Integer.vpp			--整数に関わる関数を提供する
Map.vpp			--写像に関わる関数を提供する
Number.vpp		--数に関わる共通関数を定義する
Product.vpp		--組に関わる関数を定義する
Queue.vpp			--待ち行列に関する関数を定義する
Real.vpp			--実数に関わる関数を提供する
Sequence.vpp		--列に関わる関数を提供する
Set.vpp			--集合に関わる関数を提供する
String.vpp			--文字列に関わる関数を提供する
TestDriver.vpp		--回帰テスト支援ライブラリの本体。テストケースの実行に関わる関数を提供する
TestDriverAll.tex		--TestDriverモジュールのドキュメント兼論文兼VDM++清書ソースの最上位ソース
TestLogger.vpp		--回帰テストのログを管理する関数を提供する

一時的に使用するファイル
*.dvi				--*.texをコンパイルしたdviファイル
*.pdf				--dviファイルから生成されたpdfファイル
*.vpp.aux			--TeXが生成する作業用ファイル
*.vpp.log			--TeXが生成するログ
*.vpp.tex			--VDMToolsが生成するVDM++ソースの清書文書
TestDriverAll.idx		--TeXが生成する索引用ファイル
TestDriverAll.tag		--TeXが生成するデバッグ用ファイル

フォルダーの説明
test		--単体テスト用モジュールを格納する