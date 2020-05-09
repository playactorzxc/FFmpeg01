/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               创建：夏曹俊，此代码可用作为学习参考                       **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : FFmpeg
** Description   : FFMPEG项目创建示例
** Contact       : xiacaojun@qq.com
**        博客   : http://blog.csdn.net/jiedichina
**		视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun		
				   腾讯课堂		https://jiedi.ke.qq.com/				
				   csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
**                 51cto学院	http://edu.51cto.com/lecturer/index/user_id-12016059.html	
** 				   下载最新的ffmpeg版本 ffmpeg.club
**                 
**   安卓流媒体播放器 课程群 ：23304930 加入群下载代码和交流
**   微信公众号  : jiedi2007
**		头条号	 : 夏曹俊
**
*******************************************************************************/
//！！！！！！！！！ 加群23304930下载代码和交流


//
// Created by Administrator on 2018-03-04.
//

#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"

class CXTexture:public XTexture
{
public:
    XShader sh;

    virtual bool Init(void *win)
    {
        if(!win)
        {
            XLOGE("XTexture Init failed win is NULL");
            return false;
        }
        if(!XEGL::Get()->Init(win))return false;
        sh.Init();
        return true;
    }
    virtual void Draw(unsigned char *data[],int width,int height)
    {
        sh.GetTexture(0,width,height,data[0]);  // Y
        sh.GetTexture(1,width/2,height/2,data[1]);  // U
        sh.GetTexture(2,width/2,height/2,data[2]);  // V
        sh.Draw();
        XEGL::Get()->Draw();
    }

};

XTexture *XTexture::Create()
{
    return  new CXTexture();
}