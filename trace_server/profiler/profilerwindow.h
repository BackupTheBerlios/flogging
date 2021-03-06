#pragma once
#include <QWidget>
#include <QTreeView>
#include <map>
#include "profilerblockinfo.h"
#include "rvps.h"
#include "dock.h"
#include "profilerview.h"
#include "plotconfig.h"
#include "treemodel.h"
#include "profilersessionstate.h"

QT_FORWARD_DECLARE_CLASS(QGraphicsScene)
QT_FORWARD_DECLARE_CLASS(ProfilerMainWindow)

namespace profiler {

	struct GfxView {
		QGraphicsScene * m_scene;
		View * m_view;
		QDockWidget * m_wd;

		GfxView () { memset(this, 0, sizeof(*this)); }
	};

	class View;

	class ProfilerWindow : public QObject
	{
		Q_OBJECT
	public:
		ProfilerWindow (QMainWindow * w, QObject * parent = 0, profiler::profiler_rvp_t * rvp = 0);
		~ProfilerWindow ();
		SessionState & sessionState () { return m_session_state; }
		SessionState const & sessionState () const { return m_session_state; }
	
	public slots:
		void incomingProfilerData (profiler::profiler_rvp_t * rvp);

	private:
		void setupMatrix ();
		void registerTag (BlockInfo const & bi);
		void appendToTagTree (QString const & tagpath);
		void loadState ();
		void onFileColOrExp (QModelIndex const &, bool collapsed);
		void onFileExpanded (QModelIndex const &);
		void onFileCollapsed (QModelIndex const &);

		GfxView & viewAt (size_t i);

		GfxView mkGfxView (int i);
		
		QMainWindow	* m_window;
		DockManager m_docks;
		plot::PlotConfig m_config;
		std::vector<GfxView> m_views;

		typedef tree_filter<TreeModelItem> prof_filters_t;
		prof_filters_t m_prof_filters;
		TreeModel * m_tag_model;
		profiler::profiler_rvp_t * m_rvp;
		QTreeView * m_tagWidget;
		SessionState m_session_state;

		static size_t const m_max_unique_colors = 256;
		std::vector<QColor> m_unique_colors;

		typedef std::map<std::string, QColor> colormap_t;
		colormap_t m_tagcolors;
		std::map<unsigned, unsigned> m_max_layers;
	};

	/*inline DataPlot::DataPlot (QObject * parent, plot::PlotConfig & config, QString const & fname)
		: m_parent(parent)
		, m_wd(0)
		, m_config(config)
		, m_plot(parent, 0, m_config, fname)
		, m_from(0)
		, m_fname(fname)
	{
		qDebug("%s this=0x%08x", __FUNCTION__, this);
	}*/

}

