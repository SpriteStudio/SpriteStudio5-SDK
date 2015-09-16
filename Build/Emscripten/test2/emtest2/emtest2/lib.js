mergeInto(LibraryManager.library, {
    MergeFile: function(x) {

		console.log( Pointer_stringify(x) );

	    FS.createPreloadedFile(
	        '/',
	        Pointer_stringify(x),
	        '/data/' + Pointer_stringify(x),  // httpでアクセスする際のURLを指定
	        true,
	        false
	    );

    }
});
