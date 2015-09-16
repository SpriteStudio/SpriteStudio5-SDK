Module['preRun'] = function () {
/*
	//ここでsspjを捜査して使うファイルをイメージとして生成してしまえばＯＫのはず
    FS.createFolder(
        '/', // 親フォルダの指定
        'GLSL', // フォルダ名
        true, // 読み込み許可
        true // 書き込み許可(今回の例はfalseでもよさげ)
    );
	
    FS.createPreloadedFile(
        '/GLSL',
        'sprite.vs',
        'GLSL/sprite.vs',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/GLSL',
        'sprite.fs',
        'GLSL/sprite.fs',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/GLSL',
        'sprite.fs',
        'GLSL/sprite_pot.fs',  // httpでアクセスする際のURLを指定
        true,
        false
    );
*/

    FS.createPreloadedFile(
        '/',
        'input.sspj',
        'data/NewProject.sspj',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr01.ssae',
        'data/chr01.ssae',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr_00.ssce',
        'data/chr_00.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr_00.png',
        'data/chr_00.png',  // httpでアクセスする際のURLを指定
        true,
        false
    );	
};
