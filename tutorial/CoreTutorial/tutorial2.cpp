#include "Tutorial.h"

GUI_APP_MAIN
{
	DO(Logging);
	DO(StringTutorial);
	DO(StringBufferTutorial);
	DO(WStringTutorial);
	DO(DateTime);
	DO(AsStringTutorial);
	DO(CombineHashTutorial);
	DO(ComparableTutorial);

	DO(Vector1);
	DO(Vector2);
	DO(Transfer);
	DO(ContainerClientTypes);
	DO(ArrayTutorial);
	DO(PolyArray);
	
	DO(Bidirectional);
	
	DO(IndexTutorial);
	DO(IndexClient);
	DO(Map);
	DO(OneTutorial);
	DO(AnyTutorial);
	DO(InVectorTutorial);
	DO(SortedMap);
	DO(TupleTutorial);

	DO(Range);
	DO(Algo);
	DO(Sorting);
	
	DO(FunctionTutorial);
	DO(CapturingContainers);

	DO(ValueTutorial);
	DO(NullTutorial);
	DO(Value2Tutorial);
	DO(ValueArrayMap);

	MakeTutorial();
}
