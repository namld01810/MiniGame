//
//  PlayLayer.cpp
//  SushiCrushGame
//
//  Created by MACBOOK AIR on 4/11/16.
//
//

#include "PlayLayer.hpp"
#include "SushiSprite.hpp"

#define MATRIX_WIDTH (7)
#define MATRIX_HEIGHT (10)

#define SUSHI_GAP (1)

PlayLayer::PlayLayer()
: spriteSheet(NULL)
, m_matrix(NULL)
, m_width(0)
, m_height(0)
, m_matrixLeftBottomX(0)
, m_matrixLeftBottomY(0)
, m_isNeedFillVacancies(false)
, m_isAnimationing(true) // Đặt cờ cho Animate
, m_isTouchEnable(true) // Cho phép Touch
, m_srcSushi(NULL)
, m_destSushi(NULL)
, m_movingVertical(true)  // Rơi Sushi
{
}

// Hàm hủy thì giải phóng con trỏ

PlayLayer::~PlayLayer()
{
    if (m_matrix) {
        free(m_matrix);
    }
}

// Hàm tạo Scene, đơn giản quá

Scene *PlayLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    return scene;
}

// Hàm khởi tạo init()
bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    Size winSize = Director::getInstance()->getWinSize();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto bg = Sprite::create("background.png");
    bg->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(bg);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
    spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
    addChild(spriteSheet);
    
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;
    
    m_matrixLeftBottomX = (winSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
    m_matrixLeftBottomY = (winSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;
    
    int arraySize = sizeof(SushiSprite *) * m_width * m_height;
    m_matrix = (SushiSprite **)malloc(arraySize);
    memset((void*)m_matrix, 0, arraySize);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    initMatrix(); // Khởi tạo ma trận Sushi
    scheduleUpdate();
    return true;
}
void PlayLayer::update(float dt)
{
    // Kiểm tra giá trị lần đầu của m_isAnimationing, mỗi bước thời gian dt, sẽ lại kiểm tra m_isAnimationing là true hay flase
    if (m_isAnimationing) { // nếu True
        // Gán = false
        m_isAnimationing = false;
        
        // Duyệt trong toàn ma trận
        for (int i = 0; i < m_height * m_width; i++) {
            SushiSprite *sushi = m_matrix[i];
            
            // Nếu tồn tại 1 Sushi mà đang có "Action" thì  m_isAnimationing = true, và thoát vòng lặp
            if (sushi && sushi->getNumberOfRunningActions() > 0) {
                m_isAnimationing = true;
                break;
            }
        }
    }
    
    // Đến khi không có Action nào của Sushi tại bước thời gian dt nào đó, thì kiểm tra việc "Ăn" dãy Sushi nếu tồn tại
    
    // Thiết lập cờ cho phép Touch khi không còn chuyển động, và ngược lại
    m_isTouchEnable = !m_isAnimationing;
    
    //Nếu ko có chuyển động
    if (!m_isAnimationing) {
        // Xét xem phải điền đầy ô trống không
        if (m_isNeedFillVacancies) {
            fillVacancies(); // điền đầy
            m_isNeedFillVacancies = false;
        } else {
            checkAndRemoveChain(); // Kiểm tra và ăn các chuỗi
        }
    }

}

void PlayLayer::initMatrix()
{
    
    // Duyệt các phần tử ma trận 2 chiều
    for (int row = 0; row < m_height; row++) {
        for (int col = 0; col < m_width; col++) {
            createAndDropSushi(row, col); // Tạo và làm rơi Sushi xuống vị trí hàng + cột
        }
    }
}

void PlayLayer::createAndDropSushi(int row, int col)
{
    Size size = Director::getInstance()->getWinSize();
    
    SushiSprite *sushi = SushiSprite::create(row, col); // Gọi đến hàm tạo ra Sushi của lớp SushiSprite
    
    // Tạo animation, or Action?
    Point endPosition = positionOfItem(row, col); // Lấy tọa độ Point từ row, col truyền vào
    Point startPosition = Point(endPosition.x, endPosition.y + size.height / 2); // (y) Điểm đầu = Điểm Cuối + 1 khoảng nửa màn hình
    sushi->setPosition(startPosition);
    
    float speed = startPosition.y / (2 * size.height); // tốc độ
    
    sushi->runAction(MoveTo::create(speed, endPosition)); // Di chuyển rơi xuống
    
    // Thêm vào Spritesheet
    spriteSheet->addChild(sushi);
    
    
    // Thêm sushi vào mảng, chỗ này là cách quy mảng 2 chiều về mảng 1 chiều nhé, a[i][j] = a[i*COL + j]
    
    m_matrix[row * m_width + col] = sushi;
}

// Tọa độ Point từ vị trí row, col
Point PlayLayer::positionOfItem(int row, int col)
{
    float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
    float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
    return Point(x, y);
}

void PlayLayer::checkAndRemoveChain()
{
    SushiSprite *sushi;
    // Thiết lập cờ IgnoreCheck = false
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }
        sushi->setIgnoreCheck(false);
    }
    
    // 2. Kiểm lại
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        if (!sushi) {
            continue;
        }
        
        if (sushi->getIsNeedRemove()) {
            continue; // Bỏ qua Sushi đã gắn cờ "cần loại bỏ"
        }
        if (sushi->getIgnoreCheck()) {
            continue; // Bỏ qua Sushi đã gắn cờ "bỏ qua kiểm tra"
        }
        
        // Đếm cuỗi
        std::list<SushiSprite *> colChainList;
        getColChain(sushi, colChainList);
        
        std::list<SushiSprite *> rowChainList;
        getRowChain(sushi, rowChainList);
        
        std::list<SushiSprite *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
        if (longerList.size() < 3) {
            continue;// Bỏ qua
        }
        
        std::list<SushiSprite *>::iterator itList;
        bool isSetedIgnoreCheck = false;
        for (itList = longerList.begin(); itList != longerList.end(); itList++) {
            sushi = (SushiSprite *)*itList;
            if (!sushi) {
                continue;
            }
            
            if (longerList.size() > 3) {
                // Sushi đặc biệt khi chuỗi có 4 hoặc 5 Sushi
                if (sushi == m_srcSushi || sushi == m_destSushi) {
                    isSetedIgnoreCheck = true;
                    sushi->setIgnoreCheck(true);
                    sushi->setIsNeedRemove(false);
                    
                    // Tùy theo hướng di chuyển mà tạo ra loại Sushi sọc dọc hay ngang
                    sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
                    continue;
                }
            }
            
            markRemove(sushi); // Đánh dấu cần loại bỏ sushi
        }
        
        // Chuỗi đặc biệt, khi Sushi rơi, sinh ra tự nhiên
        if (!isSetedIgnoreCheck && longerList.size() > 3) {
            sushi->setIgnoreCheck(true);
            sushi->setIsNeedRemove(false);
            sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
        }
    }
    
    // 3.Loại bỏ
    removeSushi();
}

void PlayLayer::getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
    chainList.push_back(sushi); // Thêm vào dãy Sushi đầu tiên, tại vị trí thứ i đang xét trong vòng lặp FOR của hàm checkAndRemoveChain
    
    int neighborCol = sushi->getCol() - 1; // Xét cột bên trái
    while (neighborCol >= 0) { // Tồn tại cột bên trái
        
        // Tạo 1 pointer Sushi "bên trái" trỏ vào Sushi tại vị trí  (Hàng * width + neighborCol ), đây là cách quy ma trận cấp 2  về mảng 1 chiều nhé
        SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        
        // Nếu tồn tại sushi bên trái và cùng imgIndex (cùng loại Sushi) với sushi đang xét thì..
        if (neighborSushi
            && (neighborSushi->getImgIndex() == sushi->getImgIndex())
            && !neighborSushi->getIsNeedRemove()
            && !neighborSushi->getIgnoreCheck())
 {
            // Thêm sushi trái này vào list
            chainList.push_back(neighborSushi);
            neighborCol--; // Xét tiếp Sushi bên trái đến khi ko còn Sushi nào, cột 0
        } else {
            break;  // Ko thỏa mãn đk if ở trên, Phá vòng while
        }
    }
    
    neighborCol = sushi->getCol() + 1; // Xét Sushi bên phải
    while (neighborCol < m_width) { // Xét đến cột cuối cùng, cột cuối = m_width - nhé
        // Tương tự trên tìm ông sushi cùng loại bên trái
        SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
        if (neighborSushi
            && (neighborSushi->getImgIndex() == sushi->getImgIndex())
            && !neighborSushi->getIsNeedRemove()
            && !neighborSushi->getIgnoreCheck())
 {
            chainList.push_back(neighborSushi); // Nhét vào List
            neighborCol++;
        } else {
            break; // Phá vòng while
        }
    }
}

void PlayLayer::getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
    chainList.push_back(sushi);
    
    int neighborRow = sushi->getRow() - 1; // Xét sushi bên dưới
    while (neighborRow >= 0) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi
            && (neighborSushi->getImgIndex() == sushi->getImgIndex())
            && !neighborSushi->getIsNeedRemove()
            && !neighborSushi->getIgnoreCheck())
 {
            chainList.push_back(neighborSushi);
            neighborRow--;
        } else {
            break;
        }
    }
    
    neighborRow = sushi->getRow() + 1; // Xét sushi bên trên
    while (neighborRow < m_height) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi
            && (neighborSushi->getImgIndex() == sushi->getImgIndex())
            && !neighborSushi->getIsNeedRemove()
            && !neighborSushi->getIgnoreCheck())
 {
            chainList.push_back(neighborSushi);
            neighborRow++;
        } else {
            break;
        }
    }
}


void PlayLayer::removeSushi() // Không cần truyền tham số
{
    
    m_isAnimationing = true;
    
    // Duyệt toàn ma trận
    for (int i = 0; i < m_height * m_width; i++) {
        SushiSprite *sushi = m_matrix[i];
        if (!sushi) { // Bỏ qua Sushi rỗng
            continue;
        }
        
        if (sushi->getIsNeedRemove()) { // Sushi cần xóa bỏ
            m_isNeedFillVacancies = true; // Cần điền đầy
            
            // Nổ các Sushi đặc biệt
            if(sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL) // Loại Sushi sọc ngang
            {
                explodeSpecialH(sushi->getPosition()); // Gọi hàm nổ theo chiều ngang
            }
            else if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL) // Loại Sushi sọc dọc
            {
                explodeSpecialV(sushi->getPosition()); // Gọi hàm nổ theo chiều dọc
            }
            
            explodeSushi(sushi); // Nổ sushi bình thường
            
        }
    }
    
}


void PlayLayer::explodeSushi(SushiSprite *sushi)
{
    
    // Thời gian hiệu ứng 0,3 giây
    float time = 0.3;
    
    // Thực hiện 2 hành động tuần tự, Co Sushi về kích thước, 0, 0, sau đó tự remove khỏi Contener cha
    sushi->runAction(Sequence::create(ScaleTo::create(time, 0.0),
                                      CallFuncN::create(CC_CALLBACK_1(PlayLayer::actionEndCallback, this)),NULL));
    // Action của Sprite tròn, mô phỏng vụ nổ
    
    auto circleSprite = Sprite::create("circle.png"); // Tạo mới sprite tròn
    addChild(circleSprite, 10);
    circleSprite->setPosition(sushi->getPosition()); // Vị trí = vị trí Sushi
    circleSprite->setScale(0); // Kích thước đầu =0
    // Thực hiện hành động tuần tự sau, Tăng kích thước lên tỷ lệ 1.0 trong thời gian 0,3 giây, sau đó xóa khỏi Layer
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
                                             NULL));
    
    // 3. Tạo hiệu ứng particleStars, CHÚ Ý
    
    auto particleStars = ParticleSystemQuad::create("stars.plist"); // Tạo mới
    particleStars->setAutoRemoveOnFinish(true); // Tự động remove khi xong việc
    particleStars->setBlendAdditive(false); // Thiết lập sự pha trộn thêm vào = false
    
    particleStars->setPosition(sushi->getPosition()); // Đặt vị trí tại Sushi nổ
    particleStars->setScale(0.3);  //  Thiết lập tỉ lệ 0.3
    addChild(particleStars, 20); // Thêm vào Layer Play
}


void PlayLayer::fillVacancies()
{
    Size size = CCDirector::getInstance()->getWinSize();
    // Chỗ này nhìn có vẻ phức tạp nhưng chẳng có gì đâu, chỉ là khai báo con trỏ, cấp phát bộ nhớ cho nó thôi, dùng như mảng 1 chiều
    int *colEmptyInfo = (int *)malloc(sizeof(int) * m_width);
    memset((void *)colEmptyInfo, 0, sizeof(int) * m_width); // set giá trị là 0 hết
    
    // Rơi Sushi đang có xuống khoảng trống
    SushiSprite *sushi = NULL; // Tạo 1 con trỏ Sushi = Null,
    
    // Duyệt ma trận. Lưu ý ở đây 1 chút, chúng ta thường duyệt mảng 2 chiều theo thứ tự hàng, rồi đến cột, nhưng ở đây, hơi ngược 1 tý là cột rồi đến hàng. Và lưu ý rằng Cột 0, và Hàng 0 nằm ở vị trí bên Dưới phía Trái nhé. khi tạo ma trận ta cho viên Sushi 0,0 rơi xuống trước tiên mà
    
    for (int col = 0; col < m_width; col++) { // Duyệt theo cột, từ trái sang phải
        int removedSushiOfCol = 0;
        
        // Duyệt theo hàng, từ dưới lên trên
        for (int row = 0; row < m_height; row++) {
            sushi = m_matrix[row * m_width + col]; // Sushi tại vị trí hàng, cột
            if (NULL == sushi) { // Nếu rỗng
                removedSushiOfCol++; // Đếm số Sushi đã bị "ăn"
            } else { // Nếu ko rỗng
                if (removedSushiOfCol > 0) { // Nếu bên dưới nó có ô trống = số Sushi bị ăn
                    // Làm rơi xuống
                    int newRow = row - removedSushiOfCol; //Vị trí hàng mới ( giảm xuống )
                    // Trong ma trận ta bỏ sushi ở hàng row, và chuyển nó xuống dưới qua removedSushiOfCol ô rỗng
                    m_matrix[newRow * m_width + col] = sushi;
                    m_matrix[row * m_width + col] = NULL;
                    //Di chuyển
                    Point startPosition = sushi->getPosition();
                    Point endPosition = positionOfItem(newRow, col);
                    float speed = (startPosition.y - endPosition.y) / size.height; // Tốc độ
                    sushi->stopAllActions(); // Dừng mọi chuyển động trước đó của Sushi
                    sushi->runAction(MoveTo::create(speed, endPosition)); // Di chuyển = rơi xuống
                    // set hàng mới cho Sushi tại vị trí mới này
                    sushi->setRow(newRow);
                }
            }
        }
        
        // Mảng lưu trữ số lượng Sushi bị ăn tại vị trí Cột xác định
        colEmptyInfo[col] = removedSushiOfCol;
    }
    
    // 2. Tạo mới và làm rơi các Sushi xuống khoảng trống , lấp đầy ma trận
    for (int col = 0; col < m_width; col++) { // Duyệt cột từ trái sang phải
        
        // Duyệt hàng, chỉ xét từ vị trí rỗng trở lên
        for (int row = m_height - colEmptyInfo[col]; row < m_height; row++) {
            createAndDropSushi(row, col); // Tạo Sushi và rơi xuống vị trí Row, Col
        }
    }
    m_movingVertical = true;
    m_isAnimationing = true;
    
    free(colEmptyInfo); // Giải phóng con trỏ
}

SushiSprite *PlayLayer::sushiOfPoint(Point *point)
{
    SushiSprite *sushi = NULL;
    Rect rect = Rect(0, 0, 0, 0); // Hình chữ nhật kích thước 0,0 tại Point 0,0
    
    // Duyệt ma trận Sushi
    for (int i = 0; i < m_height * m_width; i++) {
        sushi = m_matrix[i];
        
        // Tính kích thước hình chữ nhật bao quanh Sushi
        if (sushi) {
            rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);
            rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);
            rect.size = sushi->getContentSize();
            
            // Nếu hình chữ nhật đó chứa Point ( chắc là point của điểm Touch )
            if (rect.containsPoint(*point)) {
                return sushi; // trả lại Sushi
            }
        }
    }
    
    return NULL; // Trả lại Null nếu Touch ra ngoài ma trận, điểm Touch ko thuộc 1 Sushi nào
}

bool PlayLayer::onTouchBegan(Touch *touch, Event *unused)
{
    m_srcSushi = NULL; // Sushi nguồn
    m_destSushi = NULL; // Sushi dích, dùng để Swap cho nhau
    if (m_isTouchEnable) { // cho phép Touch, khi chưa ăn thì cho phép Touch
        auto location = touch->getLocation(); // lấy điểm Touch
        m_srcSushi = sushiOfPoint(&location); // Trả về Sushi tại điểm Touch
    }
    return m_isTouchEnable;
}

// Di chuyển Sushi
void PlayLayer::onTouchMoved(Touch *touch, Event *unused)
{
    if (!m_srcSushi || !m_isTouchEnable) { // Nếu Touch ra ngoài ( ko chứa Sushi nào ) hoặc ko được phép Touch
        return;
    }
    
    // Lấy vị trí Row, Col của Sushi của Sushi nguồn
    int row = m_srcSushi->getRow();
    int col = m_srcSushi->getCol();
    
    auto location = touch->getLocation();
    
    // 1/2 Chiều rộng và 1/2 chiều cao
    auto halfSushiWidth = m_srcSushi->getContentSize().width / 2;
    auto halfSushiHeight = m_srcSushi->getContentSize().height / 2;
    
    
    // Hướng di chuyển
    
    // Khung chữ nhật "phía trên Sushi nguồn"
    auto  upRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
                        m_srcSushi->getPositionY() + halfSushiHeight,
                        m_srcSushi->getContentSize().width,
                        m_srcSushi->getContentSize().height);
    
    // Nếu khung này chứa điểm Touch, nghĩa là ta sẽ di chuyển 1 Sushi đi lên trên
    if (upRect.containsPoint(location)) {
        row++; // Hàng trên của Sushi Nguồn
        if (row < m_height) {
            m_destSushi = m_matrix[row * m_width + col]; // Lấy Sushi đích
        }
        m_movingVertical = true; // Di chuyển dọc = true
        swapSushi(); // Đảo 2 Sushi nguồn và đích cho ngau
        return; // Kết thúc hàm
    }
    
    // Khung chữ nhật "phía dưới Sushi nguồn", vì sao có halfSushiHeight * 3, bạn hãy vẽ hình ra cho dễ hình dung là nhớ là tọa độ gốc của hình Rectang là điểm Left - Bottom nhé, chiều cao + rộng sẽ dựng lên theo trục X ( sang phải ), và trục Y ( lên trên ). OK??
    auto  downRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
                          m_srcSushi->getPositionY() - (halfSushiHeight * 3),
                          m_srcSushi->getContentSize().width,
                          m_srcSushi->getContentSize().height);
    
    // Chứa Touch
    if (downRect.containsPoint(location)) {
        row--; // Hàng dưới
        if (row >= 0) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = true;
        swapSushi();
        return;
    }
    
    // Các bước di chuyển sang trái, sang phải, ở đoạn code bên dưới cũng giải thích như trên các bạn nhé
    auto  leftRect = Rect(m_srcSushi->getPositionX() - (halfSushiWidth * 3),
                          m_srcSushi->getPositionY() - halfSushiHeight,
                          m_srcSushi->getContentSize().width,
                          m_srcSushi->getContentSize().height);
    
    if (leftRect.containsPoint(location)) {
        col--;
        if (col >= 0) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = false;
        swapSushi();
        return;
    }
    
    auto  rightRect = Rect(m_srcSushi->getPositionX() + halfSushiWidth,
                           m_srcSushi->getPositionY() - halfSushiHeight,
                           m_srcSushi->getContentSize().width,
                           m_srcSushi->getContentSize().height);
    
    if (rightRect.containsPoint(location)) {
        col++;
        if (col < m_width) {
            m_destSushi = m_matrix[row * m_width + col];
        }
        m_movingVertical = false;
        swapSushi();
        return;
    }
    
}


void PlayLayer::swapSushi()
{
    m_isAnimationing = true; // cho phép Animation
    m_isTouchEnable = false; // Dừng Touch
    
    if (!m_srcSushi || !m_destSushi) { // Ko tồn tại 1 trong 2 Sushi để đảo nhau
        m_movingVertical = true;
        return;
    }
    // Lấy tọa độ Point của 2 loại Sushi được đảo
    Point posOfSrc = m_srcSushi->getPosition();
    Point posOfDest = m_destSushi->getPosition();
    float time = 0.2;
    
    // 1.Hoán vị hàng, cột 2 Sushi trong ma trận, tham số quan trọng nhất là Row và Col của Sushi
    m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
    m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
    int tmpRow = m_srcSushi->getRow();
    int tmpCol = m_srcSushi->getCol();
    m_srcSushi->setRow(m_destSushi->getRow());
    m_srcSushi->setCol(m_destSushi->getCol());
    m_destSushi->setRow(tmpRow);
    m_destSushi->setCol(tmpCol);
    
    // 2.Kiểm tra xem có dãy >= 3 Sushi giống nhau được tạo ra bởi 2 Sushi sau hoán đổi này ko
    std::list<SushiSprite *> colChainListOfFirst;
    getColChain(m_srcSushi, colChainListOfFirst);
    
    std::list<SushiSprite *> rowChainListOfFirst;
    getRowChain(m_srcSushi, rowChainListOfFirst);
    
    std::list<SushiSprite *> colChainListOfSecond;
    getColChain(m_destSushi, colChainListOfSecond);
    
    std::list<SushiSprite *> rowChainListOfSecond;
    getRowChain(m_destSushi, rowChainListOfSecond);
    
    if (colChainListOfFirst.size() >= 3
        || rowChainListOfFirst.size() >= 3
        || colChainListOfSecond.size() >= 3
        || rowChainListOfSecond.size() >= 3) {
        
        // Animation đảo vị trí cho nhau
        m_srcSushi->runAction(MoveTo::create(time, posOfDest));
        m_destSushi->runAction(MoveTo::create(time, posOfSrc));
        return;
    }
    
    // 3.Không tạo được chuỗi, Đảo trở lại vị trí cũ
    m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
    m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
    tmpRow = m_srcSushi->getRow();
    tmpCol = m_srcSushi->getCol();
    m_srcSushi->setRow(m_destSushi->getRow());
    m_srcSushi->setCol(m_destSushi->getCol());
    m_destSushi->setRow(tmpRow);
    m_destSushi->setCol(tmpCol);
    
    // Di chuyển 2 bước, đảo vị trí, rồi trở lại vị trí cũ
    m_srcSushi->runAction(Sequence::create(
                                           MoveTo::create(time, posOfDest),
                                           MoveTo::create(time, posOfSrc),
                                           NULL));
    m_destSushi->runAction(Sequence::create(
                                            MoveTo::create(time, posOfSrc),
                                            MoveTo::create(time, posOfDest),
                                            NULL));
}

void PlayLayer::actionEndCallback(Node *node)
{
    // Loại bỏ Sushi khỏi ma trận và Layer
    SushiSprite *sushi = (SushiSprite *)node;
    m_matrix[sushi->getRow() * m_width + sushi->getCol()] = NULL;
    sushi->removeFromParent();
}

void PlayLayer::explodeSpecialH(Point point)
{
    Size size = Director::getInstance()->getWinSize();
    
    // Tham số để tạo hiệu ứng thôi
    float scaleX = 4 ;
    float scaleY = 0.7 ;
    float time = 0.3;
    Point startPosition = point; // điểm đầu
    float speed = 0.6f;
    
    auto colorSpriteRight = Sprite::create("colorHRight.png");
    addChild(colorSpriteRight, 10);
    Point endPosition1 = Point(point.x - size.width, point.y); // Điểm cuối
    colorSpriteRight->setPosition(startPosition);
    
    // Chỗ này thực hiện 3 hành động, kéo dãn theo X + co lại theo Y, - >chạy sang trái -> xóa khỏi layer
    colorSpriteRight->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                                 MoveTo::create(speed, endPosition1),
                                                 CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteRight)),
                                                 NULL));
    
    // Giải thích như trên
    auto colorSpriteLeft = Sprite::create("colorHLeft.png");
    addChild(colorSpriteLeft, 10);
    Point endPosition2 = Point(point.x + size.width, point.y);
    colorSpriteLeft->setPosition(startPosition);
    colorSpriteLeft->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                                MoveTo::create(speed, endPosition2),
                                                CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteLeft)),
                                                NULL));
    
    
}

void PlayLayer::explodeSpecialV(Point point)
{
    Size size = Director::getInstance()->getWinSize();
    float scaleY = 4 ;
    float scaleX = 0.7 ;
    float time = 0.3;
    Point startPosition = point;
    float speed = 0.6f;
    
    auto colorSpriteDown = Sprite::create("colorVDown.png");
    addChild(colorSpriteDown, 10);
    Point endPosition1 = Point(point.x , point.y - size.height);
    colorSpriteDown->setPosition(startPosition);
    colorSpriteDown->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                                MoveTo::create(speed, endPosition1),
                                                CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteDown)),
                                                NULL));
    
    auto colorSpriteUp = Sprite::create("colorVUp.png");
    addChild(colorSpriteUp, 10);
    Point endPosition2 = Point(point.x , point.y + size.height);
    colorSpriteUp->setPosition(startPosition);
    colorSpriteUp->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                              MoveTo::create(speed, endPosition2),
                                              CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteUp)),
                                              NULL));
}

void PlayLayer::markRemove(SushiSprite *sushi)
{
    if (sushi->getIsNeedRemove()) {
        return;
    }
    if (sushi->getIgnoreCheck()) {
        return;
    }
    
    // Set true
    sushi->setIsNeedRemove(true);
    // Các sushi loại sọc dọc
    if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL) {
        for (int row = 0; row < m_height; row++) {
            SushiSprite *tmp = m_matrix[row * m_width + sushi->getCol()];
            if (!tmp || tmp == sushi) {
                continue; //Bỏ qua loại sọc dọc
            }
            
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
                tmp->setIsNeedRemove(true); // Đánh dấu loại Sushi thường
            } else {
                markRemove(tmp); // Đệ quy,
            }
        }
        // Các sushi loại sọc ngang, tương tự
    } else if (sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL) {
        for (int col = 0; col < m_width; col++) {
            SushiSprite *tmp = m_matrix[sushi->getRow() * m_width + col];
            if (!tmp || tmp == sushi) {
                continue;
            }
            
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
                tmp->setIsNeedRemove(true);
            } else {
                markRemove(tmp);
            }
        }
    }
}
