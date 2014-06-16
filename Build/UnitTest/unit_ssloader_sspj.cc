//--------------------------------------------------------
// sspj読み込みの単体テスト
//--------------------------------------------------------

#include "gtest/gtest.h"
#include "../loader/ssloader_sspj.h"

// Loadのテスト
TEST(ssloader_sspj_test,Load) {

	SsProject* proj = ssloader_sspj::Load( "testdata/senjyu2.sspj" );

	//ロード失敗
	ASSERT_TRUE( NULL != proj );

	//
	EXPECT_EQ( 4 , proj->getAnimePackNum() );
	EXPECT_EQ( 1 , proj->getCellMapNum() );

	//SettingがXMLドキュメントと同一か確認する
	ASSERT_TRUE( proj->getProjectSetting().animeBaseDirectory == "hoge" );
	ASSERT_TRUE( proj->getProjectSetting().cellMapBaseDirectory == "" );
	ASSERT_TRUE( proj->getProjectSetting().imageBaseDirectory == "" );
	ASSERT_TRUE( proj->getProjectSetting().exportBaseDirectory == "" );
	ASSERT_TRUE( proj->getProjectSetting().queryExportBaseDirectory );
	
	//アニメファイル名リストとアニメファイルのリストの最終的な数量を確認する
	ASSERT_TRUE( proj->getAnimePackNum() == proj->getAnimePackList().size() );

}
