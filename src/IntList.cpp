#include "Essential.h"
#include "Expression.h"
#include "Symmetries.h"
#include "IntList.h"

void IntList::print()
{
	if (_item != NotSet)
		log("%d ", _item);

	bool something  = false;
	
    for (IntList* p = head(); p; p = p->next())
    {
		if (!something)
			something = true;
		
		p->print();
    }

	if (something)
		ilog("");
}

//eof
