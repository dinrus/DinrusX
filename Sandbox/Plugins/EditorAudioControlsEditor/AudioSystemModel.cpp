// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"

#include "AudioSystemModel.h"
#include "IAudioSystemEditor.h"
#include "IAudioSystemItem.h"
#include "AudioControlsEditorPlugin.h"
#include "ImplementationUpr.h"

#include <DinrusX/DinrusXSys/File/CryFile.h>  // Includes CryPath.h in correct order.
#include <QtUtil.h>

#include <QMimeData>
#include <QDataStream>
#include <QIcon>
#include <QColor>

namespace ACE
{

char const* const QAudioSystemModel::ms_szMimeType = "application/cryengine-audioimplementationitem";

QAudioSystemModel::QAudioSystemModel()
	: m_pAudioSystem(CAudioControlsEditorPlugin::GetAudioSystemEditorImpl())
{
	connect(CAudioControlsEditorPlugin::GetImplementationManger(), &CImplementationUpr::ImplementationChanged, [&]()
		{
			m_pAudioSystem = CAudioControlsEditorPlugin::GetAudioSystemEditorImpl();
			beginResetModel();
			endResetModel();
	  });
}

int QAudioSystemModel::rowCount(const QModelIndex& parent) const
{
	if (m_pAudioSystem)
	{
		IAudioSystemItem* pItem = ItemFromIndex(parent);
		if (!pItem)
		{
			// if not valid it must be a top level item so get root
			pItem = m_pAudioSystem->GetRoot();
		}

		if (pItem)
		{
			return pItem->ChildCount();
		}
	}
	return 0;
}

int QAudioSystemModel::columnCount(const QModelIndex& parent) const
{
	return eAudioSystemColumns_Count;
}

QVariant QAudioSystemModel::data(const QModelIndex& index, int role) const
{
	if (m_pAudioSystem)
	{
		if (!index.isValid())
		{
			return QVariant();
		}

		IAudioSystemItem* pItem = ItemFromIndex(index);
		if (pItem)
		{
			switch (index.column())
			{
			case eAudioSystemColumns_Name:
				{
					switch (role)
					{
					case Qt::DisplayRole:
						return (const char*)pItem->GetName();
					case Qt::DecorationRole:
						return QIcon((QtUtil::ToQString(PathUtil::GetEnginePath()) + PathUtil::GetSlash()) + m_pAudioSystem->GetTypeIcon(pItem->GetType()));
					case Qt::ForegroundRole:
						if (pItem->IsLocalised())
						{
							return QColor(36, 180, 245);
						}
						else if (!pItem->IsConnected())
						{
							return QColor(255, 143, 0);
						}
						break;
					case Qt::ToolTipRole:
						if (pItem->IsLocalised())
						{
							return tr("Item is localized");
						}
						else if (!pItem->IsConnected())
						{
							return tr("Item is not connected to any ATL control");
						}
						break;
					case eAudioSystemAttributes_Type:
						return pItem->GetType();
					case eAudioSystemAttributes_Connected:
						return pItem->IsConnected();
					case eAudioSystemAttributes_Placeholder:
						return pItem->IsPlaceholder();
					case eAudioSystemAttributes_Localized:
						return pItem->IsLocalised();
					}
				}
				break;
			}
		}
	}
	return QVariant();
}

QVariant QAudioSystemModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return tr("Name");
	}
	return QVariant();
}

Qt::ItemFlags QAudioSystemModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flag = QAbstractItemModel::flags(index);
	if (index.isValid() && m_pAudioSystem)
	{
		IAudioSystemItem* pItem = ItemFromIndex(index);
		if (pItem && !pItem->IsPlaceholder() && m_pAudioSystem->ImplTypeToATLType(pItem->GetType()) != eACEControlType_NumTypes)
		{
			flag |= Qt::ItemIsDragEnabled;
		}
	}
	return flag;
}

QModelIndex QAudioSystemModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (m_pAudioSystem)
	{
		if ((row >= 0) && (column >= 0))
		{
			IAudioSystemItem* pParent = ItemFromIndex(parent);
			if (!pParent)
			{
				pParent = m_pAudioSystem->GetRoot();
			}
			if (pParent)
			{
				IAudioSystemItem* pItem = pParent->GetChildAt(row);
				if (pItem)
				{
					return createIndex(row, column, reinterpret_cast<quintptr>(pItem));
				}
			}
		}
	}
	return QModelIndex();
}

QModelIndex QAudioSystemModel::parent(const QModelIndex& index) const
{
	if (index.isValid())
	{
		IAudioSystemItem* pItem = ItemFromIndex(index);
		if (pItem)
		{
			return IndexFromItem(pItem->GetParent());
		}
	}
	return QModelIndex();

}

Qt::DropActions QAudioSystemModel::supportedDragActions() const
{
	return Qt::CopyAction;
}

QStringList QAudioSystemModel::mimeTypes() const
{
	QStringList list = QAbstractItemModel::mimeTypes();
	list << ms_szMimeType;
	return list;
}

QMimeData* QAudioSystemModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* pData = QAbstractItemModel::mimeData(indexes);
	QByteArray byteArray;
	QDataStream stream(&byteArray, QIODevice::ReadWrite);

	for (auto index : indexes)
	{
		IAudioSystemItem* pItem = ItemFromIndex(index);
		if (pItem)
		{
			stream << pItem->GetId();
		}
	}
	pData->setData(ms_szMimeType, byteArray);
	return pData;
}

IAudioSystemItem* QAudioSystemModel::ItemFromIndex(const QModelIndex& index) const
{
	if ((index.row() < 0) || (index.column() < 0))
	{
		return nullptr;
	}
	return static_cast<IAudioSystemItem*>(index.internalPointer());
}

QModelIndex QAudioSystemModel::IndexFromItem(IAudioSystemItem* pItem) const
{
	if (pItem)
	{
		IAudioSystemItem* pParent = pItem->GetParent();
		if (!pParent)
		{
			pParent = m_pAudioSystem->GetRoot();
		}

		if (pParent)
		{
			const int size = pParent->ChildCount();
			for (int i = 0; i < size; ++i)
			{
				if (pParent->GetChildAt(i) == pItem)
				{
					return createIndex(i, 0, reinterpret_cast<quintptr>(pItem));
				}
			}
		}
	}
	return QModelIndex();
}

void QAudioSystemModel::Reset()
{
	beginResetModel();
	endResetModel();
}

QAudioSystemModelProxyFilter::QAudioSystemModelProxyFilter(QObject* parent)
	: QDeepFilterProxyModel(QDeepFilterProxyModel::BehaviorFlags(QDeepFilterProxyModel::AcceptIfChildMatches), parent)
	, m_allowedControlsMask(std::numeric_limits<uint>::max())
	, m_bHideConnected(false)
{
}

bool QAudioSystemModelProxyFilter::rowMatchesFilter(int source_row, const QModelIndex& source_parent) const
{

	if (QDeepFilterProxyModel::rowMatchesFilter(source_row, source_parent))
	{
		QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
		if (index.isValid())
		{
			// Hide placeholder
			if (sourceModel()->data(index, QAudioSystemModel::eAudioSystemAttributes_Placeholder).toBool())
			{
				return false;
			}
			if (m_bHideConnected)
			{
				if (sourceModel()->data(index, QAudioSystemModel::eAudioSystemAttributes_Connected).toBool())
				{
					return false;
				}
			}
			return sourceModel()->data(index, QAudioSystemModel::eAudioSystemAttributes_Type).toInt() & m_allowedControlsMask;
		}
	}
	return false;
}

void QAudioSystemModelProxyFilter::SetAllowedControlsMask(uint allowedControlsMask)
{
	m_allowedControlsMask = allowedControlsMask;
	invalidate();
}

void QAudioSystemModelProxyFilter::SetHideConnected(bool bHideConnected)
{
	m_bHideConnected = bHideConnected;
	invalidate();
}
}
