package fyuilib.luoc.com.util;

import android.app.Activity;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import fyuilib.luoc.com.activity.R;
import fyuilib.luoc.com.control.CTabLayoutUI;
import fyuilib.luoc.com.control.UIAttribute;
import fyuilib.luoc.com.control.UIManagerDelegate;
import fyuilib.luoc.com.core.CXmlParse;
import fyuilib.luoc.com.core.UIDlgBuild;
import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/23.
 */

public class FyUIActivity extends AppCompatActivity implements UIManagerDelegate {
    protected UIManager m_pManagerUI;

    @Override
    public boolean OnNotify(UIAttribute pSender, int event, Object param) {
        return false;
    }

    public String getUIXml(){
        return "";
    }

    public void Init(){}
    public void ContentViewInit(){
        setContentView(R.layout.activity_main);
    }

    public void InitControl(){}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);

        ContentViewInit();
        Init();

        m_pManagerUI = new UIManager(this);
        UIDlgBuild build = new UIDlgBuild();

        View v = build.Create(getUIXml(), m_pManagerUI, findViewById(android.R.id.content), this);
        ViewGroup.LayoutParams lp  = v.getLayoutParams();
        lp.width = UIManager.sScreenW;lp.height = UIManager.sScreenH - UIManager.getStatusBarHeight(this);
        v.setLayoutParams(lp);
        m_pManagerUI.SetMainView(v);

        InitControl();
    }
}
