
// SwitchMonitorDlg.h : 头文件
//

#pragma once


// CSwitchMonitorDlg 对话框
class CSwitchMonitorDlg : public CDialogEx
{
// 构造
public:
	CSwitchMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SWITCHMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	sqlite3* Global_KeyDb;
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSelect();
};
