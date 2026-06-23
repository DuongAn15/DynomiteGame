#include <gui/gameplayscreen_screen/GameplayScreenView.hpp>
#include <BitmapDatabase.hpp>

GameplayScreenView::GameplayScreenView()
{

}

void GameplayScreenView::setupScreen()
{
    GameplayScreenViewBase::setupScreen();
    updateGrid();
}

void GameplayScreenView::tearDownScreen()
{
    GameplayScreenViewBase::tearDownScreen();
}

void GameplayScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameplayScreenViewBase::handleClickEvent(evt);
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        presenter->handleTouchAim(evt.getX(), evt.getY());
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {
        presenter->handleTouchShoot(evt.getX(), evt.getY());
    }
}

void GameplayScreenView::handleDragEvent(const touchgfx::DragEvent& evt)
{
    GameplayScreenViewBase::handleDragEvent(evt);
    presenter->handleTouchAim(evt.getNewX(), evt.getNewY());
}

void GameplayScreenView::updateGrid()
{
    for (int row = 0; row < 12; row++)
    {
        for (int col = 0; col < 11; col++)
        {
            uint8_t color = presenter->getGridCell(row, col);
            if (color != 0)
            {
                uint16_t bmpId = BITMAP_EGG_BLUE_ID;
                switch (color)
                {
                    case 1: bmpId = BITMAP_EGG_BLUE_ID; break;
                    case 2: bmpId = BITMAP_EGG_GREEN_ID; break;
                    case 3: bmpId = BITMAP_EGG_ORANGE_ID; break;
                    case 4: bmpId = BITMAP_EGG_PINK_ID; break;
                    case 5: bmpId = BITMAP_EGG_PURPLE_ID; break;
                    case 6: bmpId = BITMAP_EGG_RED_ID; break;
                    case 7: bmpId = BITMAP_EGG_YELLOW_ID; break;
                }
                eggImages[row][col].setBitmap(touchgfx::Bitmap(bmpId));
                
                int x = col * 24 + ((row % 2) ? 12 : 0);
                int y = row * 21;
                eggImages[row][col].setXY(x, y);
                
                if (!eggImages[row][col].getParent())
                {
                    eggGrid.add(eggImages[row][col]);
                }
                eggImages[row][col].setVisible(true);
            }
            else
            {
                eggImages[row][col].setVisible(false);
            }
            eggImages[row][col].invalidate();
        }
    }
}
