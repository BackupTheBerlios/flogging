#pragma once
#include <QList>
#include <QVector>
#include <QString>
#include <QColor>
#include <vector>

// available widget types @TODO: frameview
enum E_DataWidgetType {
	  e_data_log
	, e_data_plot
	, e_data_table
	, e_data_gantt
	, e_data_frame
	, e_data_widget_max_value
};

enum E_ApiErrors { e_InvalidItem = -1 };

enum E_FeatureStates {
	  e_FtrDisabled		= 0
	, e_FtrDataOnly		= 1
	, e_FtrEnabled		= 2
};

enum E_ReceiveMode {
	  e_RecvSync
	, e_RecvBatched
};

enum E_NodeStates {
	  e_Unchecked
	, e_PartialCheck
	, e_Checked
};

enum E_FilterMode {
	  e_Include
	, e_Exclude
	, e_max_fltmod_enum_value
};

enum E_ExprState {
	  e_ExprInvalid
	, e_ExprValid
};

enum E_TimeUnits {
	  e_Time_ms
	, e_Time_us
	, e_Time_s
	, e_Time_m
	, e_max_timeunits_enum_value
};

QString const timeUnitsStr[] = { "ms", "us", "s", "m" };
float const timeUnitsVal[] = { 0.001f, 0.000001f, 1.0f, 60.0f };

inline QString const & unitsToString (int i) { return timeUnitsStr[i]; }
inline float stringToUnitsValue (QString const & unit_str)
{
	for (size_t i = 0; i < e_max_timeunits_enum_value; ++i)
		if (timeUnitsStr[i] == unit_str)
			return timeUnitsVal[i];
	return 0.001f;
}
inline E_TimeUnits stringToUnits (QString const & unit_str)
{
	for (size_t i = 0; i < e_max_timeunits_enum_value; ++i)
		if (timeUnitsStr[i] == unit_str)
			return static_cast<E_TimeUnits>(i);
	return e_Time_ms;
}


static char fltmods[e_max_fltmod_enum_value] = { 'I', 'E' };
static char const * fltmodsStr[e_max_fltmod_enum_value] = { "Include", "Exclude" };
inline char fltModToString (E_FilterMode l) { return fltmods[l]; }
inline E_FilterMode stringToFltMod (char c) {
	for (size_t i = 0; i < e_max_fltmod_enum_value; ++i)
		if (fltmods[i] == c)
			return static_cast<E_FilterMode>(i);
	return e_Include;
}

// level modes
enum E_LevelMode {
	  e_LvlInclude = 0
	, e_LvlForceInclude = 1
	, e_max_lvlmod_enum_value
};
static char lvlmods[e_max_lvlmod_enum_value] = { 'I', 'F' };
static char const * lvlmodsStr[e_max_lvlmod_enum_value] = { "Include", "Force" };
inline char lvlModToString (E_LevelMode l) { return lvlmods[l]; }
inline E_LevelMode stringToLvlMod (char c) {
	for (size_t i = 0; i < e_max_lvlmod_enum_value; ++i)
		if (lvlmods[i] == c)
			return static_cast<E_LevelMode>(i);
	return e_LvlInclude;
}

// row modes
enum E_RowMode {
	  e_RowInclude = 0
	, e_RowForceInclude = 1
	, e_max_rowmod_enum_value
};
static char rowmods[e_max_rowmod_enum_value] = { 'I', 'F' };
static char const * rowmodsStr[e_max_rowmod_enum_value] = { "Include", "Force" };
inline char rowModToString (E_RowMode l) { return rowmods[l]; }
inline E_RowMode stringToRowMod (char c) {
	for (size_t i = 0; i < e_max_rowmod_enum_value; ++i)
		if (rowmods[i] == c)
			return static_cast<E_RowMode>(i);
	return e_RowInclude;
}


inline E_FilterMode invert (E_FilterMode m) { return m == e_Include ? e_Include : e_Exclude; }
enum E_ColorRole { e_Bg, e_Fg };

typedef QList<QString>			columns_setup_t;
typedef QList<int>				columns_sizes_t;
typedef QList<QString>			columns_align_t;
typedef QList<QString>			columns_elide_t;

typedef std::pair<QString, QString> fileline_t;
typedef unsigned long long context_t;

typedef std::vector<std::string> strings_t;

enum E_SrcStream {
	e_Stream_TCP,
	e_Stream_File,
};

enum E_SrcProtocol {
	e_Proto_TLV,
	e_Proto_CSV
};

