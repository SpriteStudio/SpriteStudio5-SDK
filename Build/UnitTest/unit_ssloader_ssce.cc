//--------------------------------------------------------
// ssae読み込みの単体テスト
//--------------------------------------------------------

#include "gtest/gtest.h"
#include "../loader/ssloader_ssce.h"

// Loadのテスト
TEST(ssloader_ssce_test,Load) {

	SsCellMap* cell = ssloader_ssce::Load( "testdata/senjyu01.ssce");

	//成功チェック
	ASSERT_TRUE( cell != 0 );
	
	//セルマップのチェック
	ASSERT_TRUE( cell->wrapMode == SsTexWrapMode::clamp );
	ASSERT_TRUE( cell->filterMode == SsTexFilterMode::linear );

	ASSERT_TRUE( cell->pixelSize.x == 1024 );
	ASSERT_TRUE( cell->pixelSize.y == 1024 );

	ASSERT_TRUE( cell->imagePath == "senjyu01.png" );
	ASSERT_TRUE( cell->name == "senjyu01" );
	
	ASSERT_TRUE( cell->overrideTexSettings == false );


	//セルのチェック
	
	EXPECT_LT( 0 , (int)cell->cells.size() );
	ASSERT_TRUE( cell->cells[0]->name == "arm_l" );
	ASSERT_TRUE( cell->cells[0]->pivot.x == 0.4296875 );
	ASSERT_TRUE( cell->cells[0]->pivot.y < -0.354 && cell->cells[0]->pivot.y > -0.356 ); 

	ASSERT_TRUE( cell->cells[0]->size.x == 128 );
	ASSERT_TRUE( cell->cells[0]->size.y == 192 );

	ASSERT_TRUE( cell->cells[0]->pos.x == 128 );
	ASSERT_TRUE( cell->cells[0]->pos.y == 0 );

	ASSERT_TRUE( cell->cells[0]->rotated == 0 );


	delete cell;
}

