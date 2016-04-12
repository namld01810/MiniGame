//
//  PlayLayer.hpp
//  SushiCrushGame
//
//  Created by MACBOOK AIR on 4/11/16.
//
//

#ifndef PlayLayer_hpp
#define PlayLayer_hpp

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class SushiSprite; // Chỗ này bạn có thể cho lên #include

class PlayLayer : public Layer
{
public:
    
    PlayLayer();
    ~PlayLayer();
    static Scene* createScene(); // Tạo màn chơi
    CREATE_FUNC(PlayLayer);
    // Khởi tạo
    virtual bool init() override;
    virtual void update(float dt) override; // Hàm này update game Scene theo thời gian dt ( 1/60 ở file AppDelegate.cpp đó)
    virtual bool onTouchBegan(Touch *touch, Event *unused) override;
    virtual void onTouchMoved(Touch *touch, Event *unused) override;
private:
    
    // Sprite Sheet để lưu các loạt ảnh tạo animation
    SpriteBatchNode *spriteSheet;
    
    // Ma trận 2 chiều dùng con trỏ cấp 2 để lưu SushiSprite* ( Hãy đọc lại phần con trỏ và mảng 2 chiều)
    SushiSprite **m_matrix;
    
    // Kích thước Ma trận, hàng, cột
    int m_width;
    int m_height;
    
    // Vị trí căn chỉnh trên màn hình ( Tọa độ Left Bottom)
    float m_matrixLeftBottomX;
    float m_matrixLeftBottomY;
    
    // Hàm tạo ma trận
    void initMatrix();
    
    // Tạo Sushi và cho rơi xuống ở vị trí hàng cột bất kỳ
    void createAndDropSushi(int row, int col);
    
    // Trả lại vị trí tọa độ Point của Sushi tại vị trí hàng + cột trong ma trận
    Point positionOfItem(int row, int col);
    
    bool m_isAnimationing; // biến kiểm tra việc đang ăn, rơi, hay hành động khác của Sushi hay không
    void checkAndRemoveChain(); // Kiểm tra và ăn dãy Sushi
    void getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList); // Kiểm tra tồn tại dãy Sushi theo cột hay không? - Lấy ra 1 List Sushi giống nhau ( kiểu &chainList là kiểu tham chiếu trong C+, dùng để thay đổi tham số truyền vào hàm thông qua việc lấy địa chỉ. Tuy giống con trỏ, nhưng nó có điểm khác con trỏ là ko phải dùng dấu *tên biến  để thao tác mà dùng trực tiếp tên biến )
    void getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList); // Kiểm tra tồn tại dãy Sushi theo hàng hay không, Lấy ra bởi List
    void removeSushi();  // Xóa bỏ List Sushi, Ăn chuỗi Sushi
    void explodeSushi(SushiSprite *sushi); // Hiệu ứng nổ khi ăn Sushi
    void fillVacancies(); // Điền đầy khoảng trống do dãy Sushi đã bị ăn mất
    
    bool m_isTouchEnable; // Cờ cho phép Touch hoặc ko
    SushiSprite *m_srcSushi; // Pointer: Sushi nguồn
    SushiSprite *m_destSushi; // Pointer: Sushi đích
    bool m_isNeedFillVacancies; // Cờ điền đầy khoảng trống
    bool m_movingVertical; // Cờ di chuyển theo chiều dọc
    
    void actionEndCallback(Node *node);     // Dừng Action ?
    void explodeSpecialH(Point point); // Nổ theo chiều ngang
    void explodeSpecialV(Point point); // Nổ theo chiều dọc
    SushiSprite *sushiOfPoint(Point *point); // Sushi ở vị trí tọa độ Point
    void swapSushi(); // Đảo 2 Sushi
    void markRemove(SushiSprite *sushi); // Đánh dấu loại bỏ
};

#endif /* defined(__PlayLayer_H__) */
