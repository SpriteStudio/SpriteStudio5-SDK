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
/*
    FS.createPreloadedFile(
        '/',
        'input.sspj',
        'data2/NewProject.sspj',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr01.ssae',
        'data2/chr01.ssae',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr_00.ssce',
        'data2/chr_00.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'chr_00.png',
        'data2/chr_00.png',  // httpでアクセスする際のURLを指定
        true,
        false
    );	
*/
/*
    FS.createPreloadedFile(
        '/',
        'input.sspj',
        'data3/character_unity.sspj',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'unitychan_3head.ssae',
        'data3/unitychan_3head.ssae',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'unitykamen_2head.ssae',
        'data3/unitykamen_2head.ssae',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'unity_2head.ssce',
        'data3/unity_2head.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'unity_3head.ssce',
        'data3/unity_3head.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'unitykamen_2head.png',
        'data3/unitykamen_2head.png',  // httpでアクセスする際のURLを指定
        true,
        false
    );	

    FS.createPreloadedFile(
        '/',
        'unitychan_3head.png',
        'data3/unitychan_3head.png',  // httpでアクセスする際のURLを指定
        true,
        false
    );	
*/


    FS.createPreloadedFile(
        '/',
        'input.sspj',
        'data4/effect_sample.sspj',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        'e001.ssae',
        'data4/e001.ssae',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        '001a.ssee',
        'data4/001a.ssee',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        '001b.ssee',
        'data4/001b.ssee',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        '001c.ssee',
        'data4/001c.ssee',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        '001d.ssee',
        'data4/001d.ssee',  // httpでアクセスする際のURLを指定
        true,
        false
    );

    FS.createPreloadedFile(
        '/',
        'ss_effect.png',
        'data4/ss_effect.png',  // httpでアクセスする際のURLを指定
        true,
        false
    );	

    FS.createPreloadedFile(
        '/',
        'ss_effect.ssce',
        'data4/ss_effect.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );
    FS.createPreloadedFile(
        '/',
        'ss_effect_c.ssce',
        'data4/ss_effect_c.ssce',  // httpでアクセスする際のURLを指定
        true,
        false
    );
};
