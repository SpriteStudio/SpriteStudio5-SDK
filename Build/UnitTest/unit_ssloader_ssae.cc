//--------------------------------------------------------
// ssae読み込みの単体テスト
//--------------------------------------------------------

#include "gtest/gtest.h"
#include "../loader/ssloader_ssae.h"

// Loadのテスト
TEST(ssloader_ssae_test,Load) {

	SsAnimePack* anime = ssloader_ssae::Load( "testdata/senjyu01.ssae");
	ASSERT_TRUE( anime != 0 );
	
	//アニメ数のチェック
	EXPECT_EQ( 1 , anime->animeList.size() );
	//使用しているセルマップ数
	EXPECT_EQ( 1 , anime->cellmapNames.size());

	
	delete anime;
}

TEST(ssloader_ssae_test,SsModel) {

	SsAnimePack* anime = ssloader_ssae::Load( "testdata/senjyu01.ssae");

	EXPECT_NE( 0 , anime->Model.partList.size() );

	EXPECT_EQ( "root" , anime->Model.partList[0]->name );
	EXPECT_EQ( 0 , anime->Model.partList[0]->arrayIndex );
	EXPECT_EQ( SsPartType::null , anime->Model.partList[0]->type );
	EXPECT_EQ( SsBlendType::mix , anime->Model.partList[0]->alphaBlendType );
	EXPECT_EQ( SsInheritType::parent , anime->Model.partList[0]->inheritType );
	EXPECT_EQ( SsBoundsType::none , anime->Model.partList[0]->boundsType );

	EXPECT_LT( 2 , (int)anime->Model.partList.size() );
	EXPECT_EQ( 1 , anime->Model.partList[1]->arrayIndex );
	EXPECT_EQ( "renza_b01" , anime->Model.partList[1]->name );
	EXPECT_EQ( SsPartType::normal , anime->Model.partList[1]->type );
	EXPECT_EQ( SsBlendType::mix , anime->Model.partList[1]->alphaBlendType );
	EXPECT_EQ( SsInheritType::parent , anime->Model.partList[1]->inheritType );
	EXPECT_EQ( SsBoundsType::none , anime->Model.partList[1]->boundsType );

	EXPECT_EQ( "renza_f" , anime->Model.partList[2]->name );
	EXPECT_EQ( 2 , anime->Model.partList[2]->arrayIndex );

	delete anime;
}

TEST(ssloader_ssae_test,SsAttributeTest) {

	SsAnimePack* anime = ssloader_ssae::Load( "testdata/senjyu01.ssae");

	//アニメが読まれているか
	EXPECT_LT( 0 , (int)anime->animeList.size() );

	//アニメの名称があっているか
	EXPECT_EQ( "anime 1_1" , anime->animeList[0]->name );

	size_t a = anime->animeList[0]->partAnimes.size();
	EXPECT_EQ( 69 , a );

	SsString s = anime->animeList[0]->partAnimes[0]->partName;
	EXPECT_EQ( "renza_b01" , s );

	//EXPECT_EQ( "CELL" , anime->animeList[0]->partAnimes[0]->attributes[0]->tag );

	//キーフレーム取得テスト
	EXPECT_EQ( 0 , anime->animeList[0]->partAnimes[0]->attributes[0]->key[0]->time );
	EXPECT_EQ( false , anime->animeList[0]->partAnimes[0]->attributes[0]->key[0]->ipType == SsInterpolationType::linear );
	EXPECT_EQ( true , anime->animeList[0]->partAnimes[0]->attributes[0]->key[0]->value.is<SsHash>() );
	EXPECT_EQ( 0 , anime->animeList[0]->partAnimes[0]->attributes[0]->key[0]->value["mapId"].get<int>() );
	EXPECT_EQ( "renza_b01" , anime->animeList[0]->partAnimes[0]->attributes[0]->key[0]->value["name"].get<SsString>() );



	SsPartAnime& p = *anime->animeList[0]->partAnimes[0];

	//キーフレーム取得テスト
	{
		SsAttribute& at = *p.attributes[1];
		//EXPECT_EQ( "POSX" , at.tag );
		{
			size_t size = at.key.size();
			EXPECT_EQ( 2 , size );
			EXPECT_EQ( 0 , at.key[0]->time );
			EXPECT_EQ( 39 , at.key[1]->time );

			float f = at.key[0]->value.get<float>();
			EXPECT_EQ( 0 , f );
		}
	}

	{
		SsAttribute& at = *p.attributes[2];
		//EXPECT_EQ( "POSY" , at.tag );
		{
			size_t size = at.key.size();
			EXPECT_EQ( 2 , size );
			EXPECT_EQ( 0 , at.key[0]->time );
			EXPECT_EQ( 39 , at.key[1]->time );

			float f = at.key[0]->value.get<float>();
			EXPECT_EQ( 218 , f );
		}
	}

	delete anime;

}


//SsValue型のテスト
TEST(ssloader_ssae_test,AttributeValue) {

	SsValue	test;

	//文字列として代入


	SsString moji = SsString("hage");
	test = SsValue( moji );

	SsString c = test.get<SsString>();
	EXPECT_EQ( "hage" , c );

	bool flag = test.is<SsString>();

	ASSERT_TRUE( test.is<SsString>() );	//型チェック


	//文字列として代入２
	test = SsValue( "hige" );
	c = test.get<SsString>();
	EXPECT_EQ( "hige" , c );

	//Int
	test = SsValue(1);
	EXPECT_EQ( 1 , test.get<int>() );
	test = SsValue(2);
	EXPECT_EQ( 2 , test.get<int>() );
	test = SsValue(4);
	EXPECT_EQ( 4 , test.get<int>() );
	ASSERT_TRUE( test.is<int>() );	//型チェック

	//float
	test = SsValue(2.357f);
	EXPECT_EQ( 2.357f , test.get<float>() );
	ASSERT_TRUE( test.is<float>() );	//型チェック

	//bool
	test = SsValue( true );
	EXPECT_EQ( true , test.get<bool>() );
	test = SsValue( false );
	EXPECT_EQ( false , test.get<bool>() );
	ASSERT_TRUE( test.is<bool>() );	//型チェック


	//配列型/hash型
	SsArray ssarray;
	ssarray.push_back(SsValue("a"));
	test = SsValue( ssarray );
	ASSERT_TRUE( test.is<SsArray>() );

	SsValue& v= test.get<SsArray>()[0];
	EXPECT_EQ( "a" , test.get<SsArray>()[0].get<SsString>() );
	
	SsHash sshash;
	sshash["v"] = SsValue("a");

	test = SsValue( sshash );
	
	EXPECT_EQ( "a" , test["v"].get<SsString>() );



}