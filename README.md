DirectX12でブロック崩しゲームを作りました。原始的なデジタルゲームをあえて難しい方法で作ることで、ゲームのオブジェクトの設計やグラフィックスパイプラインについての知識を深めたいという目的を持って取り組みました。

実行時の動画を「デモ動画.mp4」として添付しています。

力を入れた部分は、・GameObject.h ・GameObject.cpp に含まれている、ブロックやバー、ボールといったゲームに必要なオブジェクトの設計と、・Scene.h ・Scene.cppに含まれているゲームオブジェクトの呼び出し、シーンの制御です。特に、オブジェクト指向的なクラス設計に留意し、オブジェクトの描画や状態の更新に必要な処理はオブジェクト自身に持たせ、シーンの側からは簡潔な呼び出しになるよう心掛けました。また、メモリリーク対策として適切にデストラクタを設計するよう注意しました。それ以外の描画基盤に関する部分は書籍や解説記事を参考に実装しています。



開発環境：Visual Studio 2022
使用言語：C++
制作期間：2週間
参考資料：
・川野 竜一(2020). DirectX 12の魔導書 3Dレンダリングの基礎からMMDモデルを踊らせるまで. 翔泳社

・@kyooooooooma. [DirectX12] ハローワールドからFBXモデルを表示させるまで. Qiita. 
https://qiita.com/kyooooooooma/items/c43dd8b96cc104cb6713#%E3%81%AF%E3%81%98%E3%82%81%E3%81%AB
