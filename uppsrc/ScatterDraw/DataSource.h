#ifndef _ScatterDraw_DataSource_h_
#define _ScatterDraw_DataSource_h_

namespace Upp {


class DataSource {
public:
	typedef double (DataSource::*Getdatafun)(int64 id);

	DataSource() : isParam(false), isExplicit(false), key(111111) {}
	virtual ~DataSource() 				{key = 0;}	
	virtual double y(int64 id)				{NEVER();	return Null;}
	virtual double x(int64 id)				{NEVER();	return Null;}
	virtual double znx(int n, int64 id)		{NEVER();	return Null;}
	virtual double zny(int n, int64 id)		{NEVER();	return Null;}
	virtual double znFixed(int n, int64 id)	{NEVER();	return Null;}
	virtual double y(double t)				{NEVER();	return Null;}
	virtual double x(double t)				{NEVER();	return Null;}
	virtual double f(double x)				{NEVER();	return Null;}
	virtual double f(Vector<double> zn)		{NEVER();	return Null;}
	virtual int64 GetCount()			{NEVER();	return Null;}
	bool IsEmpty()						{return GetCount() == 0;}
	virtual int GetznxCount(int64 id)	{return 0;}
	virtual int GetznyCount(int64 id)	{return 0;}
	virtual int GetznFixedCount()		{return 0;}
	bool IsParam()						{return isParam;}
	bool IsExplicit()					{return isExplicit;}
	bool IsDeleted()					{return key != 111111;}

	virtual double MinY(int64& id) 		{return Min(&DataSource::y, id);}
	virtual double MinY() 				{int64 dummy;	return Min(&DataSource::y, dummy);}
	virtual double MinX(int64& id) 		{return Min(&DataSource::x, id);}	
	virtual double MinX() 				{int64 dummy;	return Min(&DataSource::x, dummy);}

	virtual double MaxY(int64& id) 		{return Max(&DataSource::y, id);}
	virtual double MaxY() 				{int64 dummy;	return Max(&DataSource::y, dummy);}
	virtual double MaxX(int64& id) 		{return Max(&DataSource::x, id);}	
	virtual double MaxX() 				{int64 dummy;	return Max(&DataSource::x, dummy);}	
	
	virtual double IsSortedY() 			{return IsSorted(&DataSource::y);}		
	virtual double IsSortedX() 			{return IsSorted(&DataSource::x);}	
	
	virtual double AvgY() 				{return Avg(&DataSource::y);}		
	virtual double AvgX() 				{return Avg(&DataSource::x);}		
	virtual double StdDevY(double avg = Null) 	{return StdDev(&DataSource::y, avg);}	
	virtual double VarianceY(double avg = Null) {return Variance(&DataSource::y, avg);}	
	virtual Vector<int64> UpperEnvelopeY(double width) 	{return UpperEnvelope(&DataSource::y, &DataSource::x, width);}	
	virtual Vector<int64> LowerEnvelopeY(double width) 	{return LowerEnvelope(&DataSource::y, &DataSource::x, width);}	
	virtual Vector<Pointf> MovingAverageY(double width) {return MovingAverage(&DataSource::y, &DataSource::x, width);}	
	virtual Vector<Pointf> SectorAverageY(double width) {return SectorAverage(&DataSource::y, &DataSource::x, width);}	
	virtual void ZeroCrossingY(bool ascending, bool descending, Vector<double> &zeros, Vector<int64> &ids) {
		return ZeroCrossing(&DataSource::y, &DataSource::x, ascending, descending, zeros, ids);}	

	enum FFT_TYPE {T_FFT = 0, T_PHASE, T_PSD};

	Upp::Vector<Pointf> FFTY(double tSample, bool frequency = false, int type = FFT_TYPE::T_FFT, bool window = false) {
		return FFT(&DataSource::y, tSample, frequency, type, window);}

	double Min(Getdatafun getdata, int64& id);
	double Max(Getdatafun getdata, int64& id);
	double Avg(Getdatafun getdata);
	double IsSorted(Getdatafun getdata);
	double StdDev(Getdatafun getdata, double avg = Null);
	double Variance(Getdatafun getdata, double avg = Null);
	Vector<int64> UpperEnvelope(Getdatafun getdataY, Getdatafun getdataX, double width);
	Vector<int64> LowerEnvelope(Getdatafun getdataY, Getdatafun getdataX, double width);
	Vector<Pointf> MovingAverage(Getdatafun getdataY, Getdatafun getdataX, double width);
	Vector<Pointf> SectorAverage(Getdatafun getdataY, Getdatafun getdataX, double width);
	void ZeroCrossing(Getdatafun getdataY, Getdatafun getdataX, bool ascending, bool descending,
							  Vector<double> &zeros, Vector<int64> &ids);
	double SinEstim_Amplitude(double avg = Null);
	bool SinEstim_FreqPhase(double &frequency, double &phase, double avg = Null);
	Vector<Pointf> FFT(Getdatafun getdata, double tSample, bool frequency = false, 
					   int type = FFT_TYPE::T_FFT, bool window = false);
		
protected:
	bool isParam, isExplicit;

private:
	int key;
	
	Vector<int64> Envelope(Getdatafun getdataY, Getdatafun getdataX, double width, bool (*fun)(double a, double b));
};

class DataSetCond : public DataSource {
private:
	DataSource *data;
	double xGreater, xLower;

public:
	DataSetCond() {data = 0;}
	DataSetCond(DataSource &_data, double _xGreater, double _xLower) {Init(_data, _xGreater, _xLower);}
	void Init(DataSource &_data, double _xGreater, double _xLower) {
		data = &_data;
		xGreater = _xGreater;
		xLower = _xLower;
	}
	bool Check(int64 id) {
		double x = data->x(id);
		if (!IsNull(xGreater) && xGreater > x)
			return false;
		if (!IsNull(xLower) && xLower < x)
			return false;
		return true;
	}
	virtual inline double y(int64 id)	{return Check(id) ? data->y(id) : Null;}
	virtual inline double x(int64 id) 	{return Check(id) ? data->x(id) : Null;}
	virtual inline double x(double t) {
		double x = data->x(t);
		if (!IsNull(xGreater) && xGreater > x)
			return Null;
		if (!IsNull(xLower) && xLower < x)
			return Null;
		return x;
	}
	virtual inline int64 GetCount()		{return data->GetCount();}
};

class CArray : public DataSource {
private:
	double *xData, *yData, *zData;
	int64 numData;
	double x0, deltaX;
	
public:
	CArray(double *yData, int numData, double x0, double deltaX) : yData(yData), numData(numData), x0(x0), deltaX(deltaX) {xData = NULL;}
	CArray(double *yData, double *xData, int numData) : yData(yData), xData(xData), numData(numData) {zData = NULL; x0 = deltaX = 0;}
	CArray(double *yData, double *xData, double *zData, int numData) : yData(yData), xData(xData), zData(zData), numData(numData) {x0 = deltaX = 0;}
	virtual inline double y(int64 id) 	{return yData[ptrdiff_t(id)];}
	virtual inline double x(int64 id) 	{return xData ? xData[ptrdiff_t(id)] : id*deltaX + x0;}
	virtual double znFixed(int n, int64 id); 
	virtual int GetznFixedCount()		{return 1;}
	virtual inline int64 GetCount()		{return numData;}
};

template <class Y>
class VectorY : public DataSource {
private:
	Vector<Y> *yData;
	double x0, deltaX;

public:
	VectorY(Vector<Y> &yData, double x0, double deltaX) : yData(&yData), x0(x0), deltaX(deltaX) {}
	virtual inline double y(int64 id)	{return (*yData)[int(id)];}
	virtual inline double x(int64 id) 	{return id*deltaX + x0;}
	virtual inline int64 GetCount()		{return yData->GetCount();}
	virtual double MinX() 				{return x0;}	
	virtual double MaxX() 				{return x0 + (yData->GetCount() - 1)*deltaX;}	
	virtual double AvgX() 				{return x0 + ((yData->GetCount() - 1)*deltaX)/2.;}
};	

template <class Y>
class ArrayY : public DataSource {
private:
	Upp::Array<Y> *yData;
	double x0, deltaX;

public:
	ArrayY(Upp::Array<Y> &yData, double x0, double deltaX) : yData(&yData), x0(x0), deltaX(deltaX) {}
	virtual inline double y(int64 id)	{return (*yData)[ptrdiff_t(id)];}
	virtual inline double x(int64 id) 	{return id*deltaX + x0;}
	virtual inline int64 GetCount()		{return yData->GetCount();}
	virtual double MinX() 				{return x0;}	
	virtual double MaxX() 				{return x0 + yData->GetCount()*deltaX;}	
	virtual double AvgX() 				{return (x0 + yData->GetCount()*deltaX)/2.;}
};

template <class Y>
class VectorVectorY : public DataSource {
private:
	Vector<Vector<Y> > *data;
	bool useRows;
	int idx, idy;
	Vector<int> idsx, idsy, idsFixed;
	int beginData;
	int64 numData;
	
public:
	VectorVectorY() : data(0), useRows(true), beginData(0), numData(Null), idx(0), idy(1) {}
	VectorVectorY(Vector<Vector<Y> > &data, int idx, int idy, 
				  Vector<int> &idsx, Vector<int> &idsy, Vector<int> &idsFixed, bool useRows = true, int beginData = 0, int numData = Null) : 
		data(&data), useRows(useRows), beginData(beginData), numData(numData) {
		Init(data, idx, idy, idsx, idsy, idsFixed, useRows, beginData, numData);
	}
	void Init(Vector<Vector<Y> > &_data, int _idx, int _idy, Vector<int> &_idsx, Vector<int> &_idsy, Vector<int> &_idsFixed, 
			  bool _useRows = true, int _beginData = 0, int _numData = Null) {
		data = &_data;
		useRows = _useRows;
		
		idx = _idx;
		idy = _idy;
		idsx = clone(_idsx);
		idsy = clone(_idsy);
		idsFixed = clone(_idsFixed);
		beginData = _beginData;
		numData = _numData;
		if (IsNull(_numData)) {
			if (!useRows) {
				if (data->IsEmpty())
					numData = 0;
				else	
					numData = (*data)[0].GetCount() - beginData;
			} else
				numData = data->GetCount() - beginData;
		}
	}
	void Init(Vector<Vector<Y> > &_data, int idx, int idy, bool _useRows = true, int _beginData = 0, int _numData = Null) {
		static Vector<int> idsVoid;
		Init(_data, idx, idy, idsVoid, idsVoid, idsVoid, _useRows, _beginData, _numData);
	}
	virtual inline double y(int64 id) {
		if (!IsNull(idy) && idy >= 0) {
			if (useRows) 
				return (*data)[beginData + int(id)][idy];
			else
				return (*data)[idy][beginData + int(id)];
		} else {
			if (GetznyCount(id) == 0)
				return Null;
			double ret = 0;
			for (int i = 0; i < GetznyCount(id); ++i) 
				ret += zny(i, id);
			return ret/GetznyCount(id);
		}
	}
	virtual inline double x(int64 id) {return useRows ? (*data)[beginData + int(id)][idx] : (*data)[idx][beginData + int(id)];}
	//virtual inline double xn(int n, int64 id) 	{return useRows ? (*data)[beginData + int(id)][ids[n]] : (*data)[ids[n]][beginData + int(id)];}
	virtual inline int64 GetCount()		{return numData;};
	virtual double znx(int n, int64 id)	{return useRows ? (*data)[beginData + int(id)][idsx[n]] : (*data)[idsx[n]][beginData + int(id)];}
	virtual double zny(int n, int64 id)	{
		if (!IsNull(idy) && idy < 0) 
			return useRows ? (*data)[beginData + int(id)][n - idy] : (*data)[n - idy][beginData + int(id)];	
		return useRows ? (*data)[beginData + int(id)][idsy[n]] : (*data)[idsy[n]][beginData + int(id)];
	}
	virtual double znFixed(int n, int64 id)	{return useRows ? (*data)[beginData + int(id)][idsFixed[n]] : (*data)[idsFixed[n]][beginData + int(id)];}
	virtual int GetznxCount()			{return idsx.GetCount();}
	virtual int GetznyCount(int64 id) {
		if (!IsNull(idy) && idy < 0) 
			return (useRows ? (*data)[beginData + int(id)].GetCount() : (*data).GetCount()) + idy;
		return idsy.GetCount();
	}
	virtual int GetznFixedCount()		{return idsFixed.GetCount();}
};

class VectorDouble : public DataSource {
private:
	const Vector<double> *xData, *yData;

public:
	VectorDouble(const Vector<double> &yData, Vector<double> &xData) : xData(&xData), yData(&yData) {}
	virtual inline double y(int64 id)	{return (*yData)[int(id)];}
	virtual inline double x(int64 id) 	{return (*xData)[int(id)];}
	virtual inline int64 GetCount()		{return min(xData->GetCount(), yData->GetCount());}
};

class ArrayDouble : public DataSource {
private:
	const Upp::Array<double> *xData, *yData;

public:
	ArrayDouble(const Upp::Array<double> &yData, Upp::Array<double> &xData) : xData(&xData), yData(&yData) {}
	virtual inline double y(int64 id)	{return (*yData)[int(id)];}
	virtual inline double x(int64 id) 	{return (*xData)[int(id)];}
	virtual inline int64 GetCount()		{return min(xData->GetCount(), yData->GetCount());}
};

class VectorPointf : public DataSource {
private:
	const Vector<Pointf> *data;

public:
	VectorPointf(const Vector<Pointf> &data) : data(&data) 	{}
	VectorPointf(Vector<Pointf> *data) : data(data) 		{}
	virtual inline double y(int64 id)	{return (*data)[int(id)].y;}
	virtual inline double x(int64 id) 	{return (*data)[int(id)].x;}
	virtual inline int64 GetCount()		{return data->GetCount();}
};	

class ArrayPointf : public DataSource {
private:
	Upp::Array<Pointf> *data;

public:
	ArrayPointf(Upp::Array<Pointf> &data) : data(&data) {}
	virtual inline double y(int64 id)	{return (*data)[int(id)].y;}
	virtual inline double x(int64 id) 	{return (*data)[int(id)].x;}
	virtual inline int64 GetCount()		{return data->GetCount();}
};	

template <class X, class Y>
class VectorMapXY : public DataSource {
private:
	VectorMap<X, Y> *data;

public:
	VectorMapXY(VectorMap<X, Y> &data) : data(&data) {}
	virtual inline double y(int64 id)	{return (*data)[int(id)];}
	virtual inline double x(int64 id) 	{return (*data).GetKey(int(id));}
	virtual inline int64 GetCount()		{return data->GetCount();}
};	

template <class X, class Y>
class ArrayMapXY : public DataSource {
private:
	ArrayMap<X, Y> *data;

public:
	ArrayMapXY(ArrayMap<X, Y> &data) : data(&data) {}
	virtual inline double y(int64 id)	{return (*data)[int(id)];}
	virtual inline double x(int64 id) 	{return (*data).GetKey(int(id));}
	virtual inline int64 GetCount()		{return data->GetCount();}
};		
 
class FuncSource : public DataSource {
private:
	double (*function)(double);

public:
	FuncSource(double (*function)(double)) : function(function) {isExplicit = true;}
	virtual inline double f(double x)	{return function(x);}
};

class FuncSourceV : public DataSource {
private:
	void (*function)(double&, double);

public:
	FuncSourceV(void (*function)(double&, double)) : function(function) {isExplicit = true;}
	virtual inline double f(double x)	{double y; function(y, x); return y;}
};

class FuncSourcePara : public DataSource {
private:
	Pointf (*function)(double);
	Pointf lastPointf;
	double lastT;
	int numPoints;
	double minT, maxT;
	
public:
	FuncSourcePara(Pointf (*function)(double), int np, double from, double to) : 
							function(function), numPoints(np), minT(from), maxT(to) {
		isParam = true; 
		lastT = Null;
	}
	virtual inline double y(double t) {
		if (IsNull(lastT) || t != lastT) {
			lastPointf = function(minT + t*(maxT-minT)/numPoints);
			lastT = t;
		}
		return lastPointf.y;
	}
	virtual inline double x(double t) {
		if (IsNull(lastT) || t != lastT) {
			lastPointf = function(minT + t*(maxT-minT)/numPoints);
			lastT = t;
		}
		return lastPointf.x;
	}
	virtual inline int64 GetCount()	{return numPoints;}
};	

typedef Callback2<double&, double> PlotExplicFunc; 
typedef Callback2<Pointf&, double> PlotParamFunc;

class PlotExplicFuncSource : public DataSource {
private:
	PlotExplicFunc function;

public:
	PlotExplicFuncSource(PlotExplicFunc &function) : function(function) {isExplicit = true;}
	virtual inline double f(double t)	{double y; function(y, t); return y;}
};	

class PlotParamFuncSource : public DataSource {
private:
	PlotParamFunc function;
	Pointf lastPointf;
	double lastT;
	int numPoints;
	double minT, maxT;

public:
	PlotParamFuncSource(PlotParamFunc function, int np, double from, double to) : 
						function(function), numPoints(np), minT(from), maxT(to) {
		isParam = true; 
		lastT = Null;
	}
	inline double y(double t) {
		if (IsNull(lastT) || t != lastT) {
			function(lastPointf, minT + t*(maxT-minT)/numPoints);
			lastT = t;
		}
		return lastPointf.y;
	}
	inline double x(double t) {
		if (IsNull(lastT) || t != lastT) {
			function(lastPointf, minT + t*(maxT-minT)/numPoints);
			lastT = t;
		}
		return lastPointf.x;
	}
	virtual inline int64 GetCount()	{return numPoints;}
};	

struct PointfLess {
	bool operator () (const Pointf& a, const Pointf& b) const { return a.x < b.x; }
};


}

#endif
