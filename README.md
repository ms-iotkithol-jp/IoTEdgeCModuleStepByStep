# IoT Edge SDK に対応するC言語モジュールを作成する 
※ 本コンテンツは2017/6/5現在の情報を元に作成しています。 
ここでは、IoT Edge SDKのブローカーアーキテクチャに対応するC言語モジュールの開発のとっかかりを解説します。 

## 手順 
1. モジュールを設計する 
2. IoT Edge SDKリポジトリをクローンする 
3. モジュールを分類する 
4. モジュールを実装する 
5. アプリを実装する 
6. ビルドと実行 

## 1．モジュールを設計する 
先ずは、IoT Edge SDKのブローカーアーキテクチャに合わせてやりたいことをモジュールに分割します。 
分割したモジュールのメッセージ受信時の動作、起動時に必要なパラメータ、モジュールがたモジュールに送信するメッセージを明らかにします。 
モジュール群がブローカーアーキテクチャ上で、想定通りにメッセージを交換し合い、なすべきことが実行でき、実現したいシナリオを達成できるかあらかじめ検証しておきましょう。 
各モジュールについて、C言語でコンパイル可能な識別子の形式で、名前を付けておきます。*XxxYyy*的な感じでネーミングしておきましょう。 
更に、各モジュールの名前に加え、やりたいシナリオの名前（こちらもC言語でコンパイル可能な識別子の形式で）も決めておきます。こちらは、仮に、*SssNnn*としておきます。

## 2. IoT Edge SDKリポジトリをクローンする 
[http://github.com/Azure/iot-edge](http://github.com/Azure/iot-edge)をクローンします。 
このリポジトリのREADME.mdに記載された内容に従って開発環境を整えます。 

## 3. モジュールを分類する 
Step 1で定義したモジュール群が、汎用性の高いもの（他の様々なシナリオで使えるか？）か、シナリオドリブンのスペシフィックなものか、という観点で分類します。 

## 4. モジュールを実装する場所を作る 
使用するモジュール群を束ねてシナリオを実行するアプリを実装するディレクトリを先ず作ります。 
iot-edge/samples の下に、*SssNnn*という名前のディレクトリを作成しておきます。その下に、
- inc 
- src 
という二つのディレクトリを作成します。 
iot-edge/samples/CMakeLists.txtの最後に、 
add_subdirectory(*SssNnn*) 
を付け加えます。 

### 汎用モジュールの場合 
iot-edge/modulesの下に、モジュールの名前と同じ文字列でディレクトリを作成します。 
そのディレクトリに、 
- inc 
- src  

というサブディレクトリを作成し、ディレクトリ直下にCMakeLists.txtというファイルを作成し、[templates/modules/*XxxYyy*/CMakeLists.txt](templates/modules/XxxYyy/CMakeLists.txt)の内容をコピペします。  
コピペしたら、XxxYyyを各モジュールの名前に全部置換します。  
inc、srcに、それぞれ、[templates/modules/*XxxYyy*/inc/*XxxYyy*.h](templates/modules/XxxYyy/inc/XxxYyy.h)、[templates/modules/*XxxYyy*/inc/*XxxYyy*.c](templates/modules/XxxYyy/inc/XxxYyy.c)をコピーし、ファイル名の変更と、ファイル内の*XxxYyy*のモジュール名への変更を行います。  
iot-edge/modules/CMakeLists.txtの最後に、
add_subdirectory(*XxxYyy*) 
を追加します。 

### スペシフィックなモジュールの場合 
シナリオを実行するアプリを実装するディレクトリの下に、モジュールの名前と同じ文字列でディレクトリを作成します。後は、汎用モジュールの時と同様に、incとsrc、CMakeLists.txt、ヘッダーファイル、ソースファイルを作成します。  
シナリオを実行するアプリを実装するディレクトリの下の、CMakeLists.txtの最後に、add_subdirectory(*XxxYyy*)の追加が必要です。 

### ロジックの実装 
*XxxYyy*.cの中で、
```c
    // TODO: ...
```
と記載された部分に、モジュールがすべきロジックを実装します。大枠は、 
- ParseConfigurationFromJson  
JSONに記述された起動変数群の読み込み
- FreeConfiguration  
起動変数群を読み込んだメモリの開放 
- Create  
モジュール生成時の初期化処理 
- Start  
モジュール開始時の処理 
- Receive  
メッセージ受信時の処理 
- Destory  
モジュール停止時の処理 
です。 

## 5． アプリの実装
シナリオを実行するアプリを実装するディレクトリ（このケースではSssNnnディレクトリ）、のsrcディレクトリに、[templates/samples/*SssNnn*/src](templates/samples/SssNnn/src)のmain.cと、*SssNnn*.json（名前は適宜変えてね）をコピペします。  
実際にロード＆実行するモジュール群や引数に合わせて、*SssNnn*.jsonの中身を変更します。main.cはほぼそのままでOKなはずです。  

## 6．ビルドと実行 
iot-edgeディレクトリにcdし、 
./tools/build.sh 
を実行します。 
追加したモジュール群とアプリがビルドされます。 
実行は、 
build/samples/*SssNnn* samples/*SssNnn*/src/*SssNnn*.json 
で行います。 
