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
// Created by Administrator on 2018-03-02.
//

#include "IDecode.h"
#include "XLog.h"

//由主体notify的数据
void IDecode::Update(XData pkt)
{
    if(pkt.isAudio != isAudio)
    {
        return;
    }
    while (!isExit)
    {
        packsMutex.lock();

        //阻塞
        if(packs.size() < maxList)
        {
            //生产者
            packs.push_back(pkt);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }


}

void IDecode::Main()
{
    while(!isExit)
    {
        packsMutex.lock();

        //判断音视频同步
        if(!isAudio && synPts > 0)
        {
            if(synPts < pts)
            {
                packsMutex.unlock();
                XSleep(1);
                continue;
            }
        }

        if(packs.empty())
        {
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        //取出packet 消费者
        XData pack = packs.front();
        packs.pop_front();

        //发送数据到解码线程，一个数据包，可能解码多个结果
        if(this->SendPacket(pack))
        {
            while(!isExit)
            {
                //获取解码数据
                XData frame = RecvFrame();
                if(!frame.data) break;
                //XLOGE("RecvFrame %d",frame.size);
                pts = frame.pts;
                //发送数据给观察者
                this->Notify(frame);

            }

        }
        pack.Drop();
        packsMutex.unlock();
    }
}