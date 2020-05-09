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

#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_WARN,"testff",__VA_ARGS__)


//顶点着色器glsl
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
    attribute vec4 aPosition; //顶点坐标
    attribute vec2 aTexCoord; //材质顶点坐标
    varying vec2 vTexCoord;   //输出的材质坐标
    void main(){
        vTexCoord = vec2(aTexCoord.x,1.0-aTexCoord.y);
        gl_Position = aPosition;
    }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragYUV420P = GET_STR(
    precision mediump float;    //精度
    varying vec2 vTexCoord;     //顶点着色器传递的坐标
    uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
    uniform sampler2D uTexture;
    uniform sampler2D vTexture;
    void main(){
        vec3 yuv;
        vec3 rgb;
        yuv.r = texture2D(yTexture,vTexCoord).r;
        yuv.g = texture2D(uTexture,vTexCoord).r - 0.5;
        yuv.b = texture2D(vTexture,vTexCoord).r - 0.5;
        rgb = mat3(1.0,     1.0,    1.0,
                   0.0,-0.39465,2.03211,
                   1.13983,-0.58060,0.0)*yuv;
        //输出像素颜色
        gl_FragColor = vec4(rgb,1.0);
    }
);

GLint InitShader(const char *code,GLint type)
{
    //创建shader
    GLint sh = glCreateShader(type);
    if(sh == 0)
    {
        LOGD("glCreateShader %d failed!",type);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,    //shader数量
                   &code, //shader代码
                   0);   //代码长度
    //编译shader
    glCompileShader(sh);

    //获取编译情况
    GLint status;
    glGetShaderiv(sh,GL_COMPILE_STATUS,&status);
    if(status == 0)
    {
        LOGD("glCompileShader failed!");
        return 0;
    }
    LOGD("glCompileShader success!");
    return sh;
}


extern "C"
JNIEXPORT jstring

JNICALL
Java_aplay_testopengles_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_aplay_testopengles_XPlay_Open(JNIEnv *env, jobject instance, jstring url_, jobject surface) {
    const char *url = env->GetStringUTFChars(url_, 0);
    LOGD("open url is %s",url);
    //1 获取原始窗口
    ANativeWindow *nwin = ANativeWindow_fromSurface(env,surface);

    ////////////////////
    ///EGL
    //1 EGL display创建和初始化
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == EGL_NO_DISPLAY)
    {
        LOGD("eglGetDisplay failed!");
        return;
    }
    if(EGL_TRUE != eglInitialize(display,0,0))
    {
        LOGD("eglInitialize failed!");
        return;
    }
    //2 surface
    //2-1 surface窗口配置
    //输出配置
    EGLConfig config;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };
    if(EGL_TRUE != eglChooseConfig(display,configSpec,&config,1,&configNum))
    {
        LOGD("eglChooseConfig failed!");
        return;
    }
    //创建surface
    EGLSurface winsurface = eglCreateWindowSurface(display,config,nwin,0);
    if(winsurface == EGL_NO_SURFACE)
    {
        LOGD("eglCreateWindowSurface failed!");
        return;
    }

    //3 context 创建关联的上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE
    };
    EGLContext context = eglCreateContext(display,config,EGL_NO_CONTEXT,ctxAttr);
    if(context == EGL_NO_CONTEXT)
    {
        LOGD("eglCreateContext failed!");
        return;
    }
    if(EGL_TRUE != eglMakeCurrent(display,winsurface,winsurface,context))
    {
        LOGD("eglMakeCurrent failed!");
        return;
    }

    LOGD("EGL Init Success!");

    //顶点和片元shader初始化
    //顶点shader初始化
    GLint vsh = InitShader(vertexShader,GL_VERTEX_SHADER);
    //片元yuv420 shader初始化
    GLint fsh = InitShader(fragYUV420P,GL_FRAGMENT_SHADER);





    env->ReleaseStringUTFChars(url_, url);
}