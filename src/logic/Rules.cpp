#include "../../include/logic/Rules.h"
#include "../../include/entity/Card.h"

Rules::Rules()
{
}

Rules::~Rules()
{
}

bool Rules::isValidMove(Card played, Card tableTop, bool isDarkMode)
{
    if (played.lightType == Type::WILD_CARD || played.lightType == Type::CUSTOM)
    {
        return true;
    }

    if (!isDarkMode)
    {
        if (played.lightColor == tableTop.lightColor || played.lightValue == tableTop.lightValue)
        {
            return true;
        }
    }
    else
    {
        if (played.darkColor == tableTop.darkColor || played.darkValue == tableTop.darkValue)
        {
            return true;
        }
    }
    return false;
}
