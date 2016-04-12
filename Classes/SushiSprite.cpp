//
//  SushiSprite.cpp
//  SushiCrushGame
//
//  Created by MACBOOK AIR on 4/11/16.
//
//

#include "SushiSprite.hpp"

USING_NS_CC;

#define TOTAL_SUSHI 6 // Tổng số loại Sushi - define theo từng màn chơi

SushiSprite::SushiSprite()
: m_col(0)
, m_row(0)
, m_imgIndex(0)
, m_isNeedRemove(false)
, m_ignoreCheck(false)
, m_displayMode(DISPLAY_MODE_NORMAL)
{
}

// Tạo 1 mảng con trỏ, mỗi con trỏ trỏ tới 1 chuỗi, sushiNormal[i] lưu địa chỉ chuỗi i
static const char *sushiNormal[TOTAL_SUSHI] = {
    "sushi_1n.png",
    "sushi_2n.png",
    "sushi_3n.png",
    "sushi_4n.png",
    "sushi_5n.png",
    "sushi_6n.png"
};

static const char *sushiVertical[TOTAL_SUSHI] = {
    "sushi_1v.png",
    "sushi_2v.png",
    "sushi_3v.png",
    "sushi_4v.png",
    "sushi_5v.png",
    "sushi_6v.png"
};

static const char *sushiHorizontal[TOTAL_SUSHI] = {
    "sushi_1h.png",
    "sushi_2h.png",
    "sushi_3h.png",
    "sushi_4h.png",
    "sushi_5h.png",
    "sushi_6h.png"
};

// Lấy chiều rộng của đối tượng
float SushiSprite::getContentWidth()
{
    static float itemWidth = 0;
    if (itemWidth==0) {
        
        // Tạo ra 1 sushi từ mảng trên
        auto sprite = Sprite::createWithSpriteFrameName(sushiNormal[0]);
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

// Tạo mới 1 Sushi có vị trí row, col, trả về 1 con trỏ kiếu SushiSprite*
SushiSprite *SushiSprite::create(int row, int col)
{
    // Tạo mới
    SushiSprite *sushi = new SushiSprite();
    // Gắn hàng, cột, index
    sushi->m_row = row;
    sushi->m_col = col;
    sushi->m_imgIndex =  rand() % TOTAL_SUSHI; // random loại Sushi từ 0-5 (= index của mảng)
    
    // Tạo hình ảnh từ chuỗi của mảng trên
    sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
    sushi->autorelease(); // Tự động hủy khi cần
    return sushi;
}
void SushiSprite::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
    
    SpriteFrame *frame;
    
    // Tùy theo các trường hợp của mode, mà tạo ra loại Sushi tương ứng mode đó
    switch (mode) {
        case DISPLAY_MODE_VERTICAL:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiVertical[m_imgIndex]);
            break;
        case DISPLAY_MODE_HORIZONTAL:
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sushiHorizontal[m_imgIndex]);
            break;
        default:
            return;
    }
    setDisplayFrame(frame); // Hàm của lớp Sprite
}