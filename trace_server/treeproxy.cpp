#include "treeproxy.h"
#include "connection.h"
#include "mainwindow.h"

TreeProxyModel::TreeProxyModel (TreeModel * src, QItemSelectionModel * selection)
	: KSelectionProxyModel(selection)
	, m_selection(selection)
	, m_src(src)
{
}


void TreeProxyModel::setFindString (QString const & s)
{
	m_find = s;

	QModelIndexList const children = m_src->find(s);

	for (int i = 0, ie = children.size(); i < ie; ++i)
		m_selection->select(children.at(i));
	//beginResetModel();
	//endResetModel();
}
