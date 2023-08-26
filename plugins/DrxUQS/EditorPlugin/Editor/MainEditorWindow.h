// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/QTViewPane.h>
<DinrusX/QLabel>
<DinrusX/QPushButton>
<DinrusX/QVBoxLayout>
<DinrusX/QHBoxLayout>
<DinrusX/QSizePolicy>
<DinrusX/QSplitter>
<DinrusX/QCheckBox>
<DinrusX/QGroupBox>

<DinrusX/IEditor.h>
<DinrusX/DockedWidget.h>
<DinrusX/Controls/EditorDialog.h>

#include "Settings.h"
#include "EditorContext.h"
#include "QuerySimulator.h"
#include "CentralEventUpr.h"

class CUqsQueryDocument;
class CUqsDatabaseSerializationCache;
class CQueryListProvider;

class QPropertyTree;

class QLineEdit;
class QDialogButtonBox;

namespace Explorer
{
struct ExplorerEntry;
class ExplorerData;
class ExplorerFileList;
class ExplorerPanel;
} // namespace Explorer

namespace UQSEditor
{
class CQueryBlueprint;
} // namespace UQSEditor

class CNewQueryDialog : public CEditorDialog
{
	Q_OBJECT

	typedef CEditorDialog BaseClass;
public:
	CNewQueryDialog(const QString& newName, QWidget* pParent);

	// CEditorDialog
	virtual void accept() override;
	// ~CEditorDialog

	const QString& GetResultingString();

private:
	QLineEdit*        m_pLineEdit;
	QDialogButtonBox* m_pButtons;
	QString           m_resultingString;
};

class CMainEditorWindow 
	: public CDockableWindow
	, public IEditorNotifyListener
{
	Q_OBJECT

private:

	struct SSimulatedRuntimeParams
	{
		void Serialize(Serialization::IArchive& archive);

		std::vector<CSimulatedRuntimeParam> params;
	};

public:

	CMainEditorWindow();
	~CMainEditorWindow();

	// CDockableWindow
	virtual const char*                       GetPaneTitle() const override;
	virtual IViewPaneClass::EDockingDirection GetDockingDirection() const override { return IViewPaneClass::DOCK_FLOAT; }
	// ~CDockableWindow

	// IEditorNotifyListener
	virtual void OnEditorNotifyEvent(EEditorNotifyEvent ev) override;
	// ~IEditorNotifyListener

protected:
	void customEvent(QEvent* event) override;

private:

	void                     CreateNewDocument();

	void                     BuildLibraryPanel();
	void                     BuildDocumentPanel();
	void                     BuildSimulatorPanel();
	void                     BuildMenu();

	void                     OnMenuActionFileNew();
	void                     OnMenuActionFileSave();
	void                     OnMenuActionFileSaveAs();

	void                     OnMenuActionViewUseSelectionHelpers(bool checked);
	void                     OnMenuActionViewShowInputParamTypes(bool checked);
	void                     OnMenuActionViewFilterInputsByType(bool checked);

	void                     OnMenuActionHelpOnlineDocumentation();

	void                     OnLibraryExplorerSelectionChanged();
	void                     OnLibraryExplorerActivated(const Explorer::ExplorerEntry* pExplorerEntry);

	void                     OnDocumentPropertyTreeChanged();

	void                     OnDocumentAboutToBeRemoved(CUqsQueryDocument* pDocument);

	void                     OnSimulatorButtonClicked(bool checked);
	void                     OnSingleStepOnceButtonClicked(bool checked);

	Explorer::ExplorerPanel* GetLibraryExplorerWidget() const;

	void                     SetCurrentDocumentFromExplorerEntry(const Explorer::ExplorerEntry* pExplorerEntry);

	void                     RebuildRuntimeParamsListForSimulation();
	void                     RebuildRuntimeParamsListForSimulationRecursively(const UQSEditor::CQueryBlueprint& queryBlueprintToRecurseFrom);

	void                     OnQueryBlueprintRuntimeParamsChanged(const CCentralEventUpr::SQueryBlueprintRuntimeParamsChangedEventArgs& args);
	void                     OnQuerySimulatorRunningStateChanged(const CCentralEventUpr::SQuerySimulatorRunningStateChangedEventArgs& args);

private:

	CUqsEditorContext                       m_editorContext;

	std::unique_ptr<Explorer::ExplorerData> m_pExplorerData;
	Explorer::ExplorerPanel*                m_pLibraryPanel;

	QTabWidget*                             m_pDocumentTabsWidget;
	QPropertyTree*                          m_pDocumentPropertyTree;
	CUqsQueryDocument*                      m_pCurrentDocument;

	QWidget*                                m_pSimulatorPanel;
	QPropertyTree*                          m_pSimulatorPropertyTree;  // contains the runtime-params of the currently selected query blueprint
	QPushButton*                            m_pSimulatorButton;
	QCheckBox*                              m_pSimulatorRunModeCheckBox;
	CQuerySimulator                         m_querySimulator;
	SSimulatedRuntimeParams                 m_simulatedRuntimeParams;
};
