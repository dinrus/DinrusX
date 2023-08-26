// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/QtViewPane.h>
<DinrusX/QLabel>
<DinrusX/QAbstractItemModel>
<DinrusX/QTextEdit>
<DinrusX/QComboBox>
<DinrusX/QPushButton>

<DinrusX/IEditor.h>
<DinrusX/DockedWidget.h>
<DinrusX/Controls/EditorDialog.h>
<DinrusX/IPostRenderer.h>

<DinrusX/DrxUQS/Shared/SharedIncludes.h>
<DinrusX/Serialization/QPropertyTree/QPropertyTree.h>

struct SQuery;
class CHistoricQueryTreeModel;
class CHistoricQueryTreeView;

class CMainEditorWindow : public CDockableWindow, public UQS::Core::IQueryHistoryListener, public UQS::Core::IQueryHistoryConsumer
{
	Q_OBJECT

private:

	class CUQSHistoryPostRenderer : public IPostRenderer
	{
	public:

		explicit CUQSHistoryPostRenderer(CHistoricQueryTreeView& historicQueryTreeView);

		// IPostRenderer
		virtual void OnPostRender() const override;
		// ~IPostRenderer

	private:

		CHistoricQueryTreeView& m_historicQueryTreeView;
	};

public:

	CMainEditorWindow();
	~CMainEditorWindow();

	// CDockableWindow
	virtual const char*                       GetPaneTitle() const override;
	virtual IViewPaneClass::EDockingDirection GetDockingDirection() const override { return IViewPaneClass::DOCK_FLOAT; }
	// ~CDockableWindow

	// ~IQueryHistoryListener
	virtual void OnQueryHistoryEvent(const UQS::Core::IQueryHistoryListener::SEvent& ev) override;
	// ~IQueryHistoryListener

	// IQueryHistoryConsumer
	virtual void AddOrUpdateHistoricQuery(const SHistoricQueryOverview& overview) override;
	virtual void AddTextLineToCurrentHistoricQuery(const ColorF& color, const char* szFormat, ...) override;
	virtual void AddTextLineToFocusedItem(const ColorF& color, const char* szFormat, ...) override;
	virtual void AddInstantEvaluatorName(const char* szInstantEvaluatorName) override;
	virtual void AddDeferredEvaluatorName(const char* szDeferredEvaluatorName) override;
	// ~IQueryHistoryConsumer

protected:
	void customEvent(QEvent* event) override;

private:
	void OnHistoryOriginComboBoxSelectionChanged(int index);
	void OnClearHistoryButtonClicked(bool checked);
	void OnSaveLiveHistoryToFile();
	void OnLoadHistoryFromFile();
	void OnTreeViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
	string                           m_windowTitle;
	UQS::Core::IQueryHistoryUpr* m_pQueryHistoryUpr;
	SQuery*                          m_pFreshlyAddedOrUpdatedQuery;
	CHistoricQueryTreeView*          m_pTreeView;
	CHistoricQueryTreeModel*         m_pTreeModel;
	QTextEdit*                       m_pTextQueryDetails;
	QTextEdit*                       m_pTextItemDetails;
	QComboBox*                       m_pComboBoxHistoryOrigin;
	QPushButton*                     m_pButtonClearCurrentHistory;
	QPropertyTree*                   m_pPropertyTree;
	CUQSHistoryPostRenderer*         m_pHistoryPostRenderer;
};
