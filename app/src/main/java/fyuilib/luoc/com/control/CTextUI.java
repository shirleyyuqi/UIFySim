package fyuilib.luoc.com.control;

import android.content.Context;
import android.graphics.Canvas;
import android.text.TextPaint;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import fyuilib.luoc.com.core.UIManager;

/**
 * Created by luoc on 2018/5/5.
 */

public class CTextUI extends TextView implements UIAttribute{
    private CCommonFunUI m_pCommonUI = null;
    private int          m_iTextIndent  = 0;

    public CTextUI(Context context, UIManager manager) {
        super(context);

        m_pCommonUI = new CCommonFunUI();
        m_pCommonUI.initWithDelegateView(this, manager);

        setTextSize(18);
    }

    @Override
    public void SetAttribute(String name, String value) {
        if (name.compareTo("text") == 0) {
            setText(m_pCommonUI.getLanString(value));
            /*NSString* str = [commonFun getLanString:value];
            [self setText:str];
            if (lineStyle != 0) {
                NSDictionary *attribtDic = @{NSUnderlineStyleAttributeName: [NSNumber numberWithInteger:lineStyle]};
                NSMutableAttributedString *attribtStr = [[NSMutableAttributedString alloc] initWithString:self.text attributes:attribtDic];
                self.attributedText = attribtStr;
            }*/
        }else if (name.compareTo("align") == 0){
            if (value.compareTo("center") == 0) {
                setGravity(Gravity.CENTER);
                //self.textAlignment = NSTextAlignmentCenter;
            }else if (value.compareTo("right") == 0){
                //self.textAlignment = NSTextAlignmentRight;
            }else{
                //self.textAlignment = NSTextAlignmentLeft;
            }
        }else if (name.compareTo("textcolor") == 0){
            //self.textColor = [CCommonFunUI getColorValue:value];
        }else if (name.compareTo("font") == 0){
            setTextSize(Integer.parseInt(value));
            //self.font = [UIFont systemFontOfSize:atof(value)];
        }else if (name.compareTo("linestyle") == 0){
            /*lineStyle = atoi(value);
            if (self.text && lineStyle != 0) {
                NSDictionary *attribtDic = @{NSUnderlineStyleAttributeName: [NSNumber numberWithInteger:lineStyle]};
                NSMutableAttributedString *attribtStr = [[NSMutableAttributedString alloc] initWithString:self.text attributes:attribtDic];
                self.attributedText = attribtStr;
            }*/
        }else if(name.compareTo("textIndent") == 0){
            m_iTextIndent = UIManager.dip2px(getContext(), Integer.parseInt(value));
            //fTextIndent = atof(value);
        }else{
            m_pCommonUI.setAttribute(name, value);
        }
    }

    @Override
    public CCommonFunUI GetCommFun() {
        return m_pCommonUI;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.translate(m_iTextIndent, (getHeight() - UIManager.getFontHeight(getTextSize())) / 2);
        super.onDraw(canvas);
    }
}
