//
//  SushiSprite.hpp
//  SushiCrushGame
//
//  Created by MACBOOK AIR on 4/11/16.
//
//

#ifndef SushiSprite_hpp
#define SushiSprite_hpp

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    DISPLAY_MODE_NORMAL = 0,
    DISPLAY_MODE_HORIZONTAL,
    DISPLAY_MODE_VERTICAL,
} DisplayMode;

class SushiSprite:public Sprite // Kế thừa từ lớp Sprite nhé

{
public:
    SushiSprite();
    static SushiSprite* create(int row, int col); // Tạo 1 Sushi tại vị trí hàng, cột thuộc Ma trận
    static float getContentWidth(); // Lấy chiều rộng của sprite sushi, cần thiết cho việc tính toán về sau
    CC_SYNTHESIZE(int,m_row,Row); // Vị trí hàng của Sushi trong Ma trận
    CC_SYNTHESIZE(int,m_col,Col);  // Vị trí hàng của Sushi trong Ma trận
    CC_SYNTHESIZE(int,m_imgIndex,ImgIndex); // Loại Sushi
    
    CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove); // Cờ đánh dấu cần loại bỏ
    CC_SYNTHESIZE(bool, m_ignoreCheck, IgnoreCheck); //  Cờ bỏ qua kiểm tra
    CC_SYNTHESIZE_READONLY(DisplayMode, m_displayMode, DisplayMode); // Mode hiển thị
    void setDisplayMode(DisplayMode mode); // Thiết lập mode
    
};


#endif /* SushiSprite_hpp */
