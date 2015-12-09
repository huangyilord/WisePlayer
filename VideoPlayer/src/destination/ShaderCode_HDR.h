//
//  ShaderCode_HDR.h
//  VideoPlayer
//
//  Created by wangyu on 15/9/28.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef GraphicDisplayFilter_HDR_Shader_h
#define GraphicDisplayFilter_HDR_Shader_h

const CHAR* s_DefaultVertexShaderString =
"uniform mat4 u_transform;"
"uniform mat4 u_texTransform;"
"attribute vec3 a_position;"
"attribute vec2 a_tex0;"
"varying mediump vec2 v_tex0;"
"void main()"
"{"
"vec4 pos = vec4( a_position, 1 );"
"gl_Position = u_transform * pos;"
"vec4 tx = vec4( a_tex0, 0, 1 );"
"v_tex0 = (u_texTransform * tx).xy;"
"}";

const CHAR* s_DefaultPixelShaderString =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"void main()"
"{"
"gl_FragColor = texture2D(u_texture, v_tex0);"
"}";

// 在X轴上的高斯模糊
const CHAR* s_blurXPixelShaderString =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"uniform int imgW;"
""
"void main()"
"{"
"    float d = 1.0/float(imgW);"
"    vec4 color = vec4(0.0,0.0,0.0,0.0);"
"    color += texture2D(u_texture,v_tex0+vec2(-5.0*d,0.0)) * 0.1;"
"    color += texture2D(u_texture,v_tex0+vec2(-4.0*d,0.0)) * 0.22;"
"    color += texture2D(u_texture,v_tex0+vec2(-3.0*d,0.0)) * 0.358;"
"    color += texture2D(u_texture,v_tex0+vec2(-2.0*d,0.0)) * 0.523;"
"    color += texture2D(u_texture,v_tex0+vec2(-1.0*d,0.0)) * 0.843;"
"    color += texture2D(u_texture,v_tex0 ) * 1.0;"
"    color += texture2D(u_texture,v_tex0+vec2( 1.0*d,0.0)) * 0.843;"
"    color += texture2D(u_texture,v_tex0+vec2( 2.0*d,0.0)) * 0.523;"
"    color += texture2D(u_texture,v_tex0+vec2( 3.0*d,0.0)) * 0.358;"
"    color += texture2D(u_texture,v_tex0+vec2( 4.0*d,0.0)) * 0.22;"
"    color += texture2D(u_texture,v_tex0+vec2( 5.0*d,0.0)) * 0.1;"
"    color /= 5.0;"
"    gl_FragData[0] = color;"
"}";

//在Y轴上的高斯模糊
const CHAR* s_blurYPixelShaderString =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"uniform int imgH;"
""
"void main()"
"{"
"    float d = 1.0/float(imgH);"
"    vec4 color = vec4(0.0,0.0,0.0,0.0);"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0,-5.0*d)) * 0.1;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0,-4.0*d)) * 0.22;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0,-3.0*d)) * 0.358;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0,-2.0*d)) * 0.563;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0,-1.0*d)) * 0.873;"
"    color +=     texture2D(u_texture,v_tex0 ) * 1.0;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0, 1.0*d)) * 0.873;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0, 2.0*d)) * 0.563;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0, 3.0*d)) * 0.358;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0, 4.0*d)) * 0.22;"
"    color +=     texture2D(u_texture,v_tex0+vec2(0.0, 5.0*d)) * 0.1;"
"    color /= 5.0;"
"    gl_FragData[0] = color;"
"}";

//Bloom.fs 产生Bloom贴图
const CHAR* s_bloomPixelShaderString =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"uniform float AveLum;"
"uniform int imgH,imgW;"
"void main()"
"{"
"    float dx = 1.0/float(imgW);"
"    float dy = 1.0/float(imgH);"
//"    //对u_texture进行采样"
"    vec4 color = texture2D(u_texture,v_tex0);"
"    color += texture2D(u_texture,v_tex0+vec2(dx*3.0,0.0));"
""
"    color += texture2D(u_texture,v_tex0+vec2(0.0,dy));"
"    color += texture2D(u_texture,v_tex0+vec2(dx*3.0,dy));"
""
"    color += texture2D(u_texture,v_tex0+vec2(0.0,dy*2.0));"
"    color += texture2D(u_texture,v_tex0+vec2(dx*3.0,dy*2.0));"
""
"    color += texture2D(u_texture,v_tex0+vec2(0.0,dy*3.0));"
"    color += texture2D(u_texture,v_tex0+vec2(dx*3.0,dy*3.0));"
"    color /= 8.0;"
//"    //计算该像素在Tone Mapping之后的亮度值，如果依然很大，则该像素将产生光晕"
"    vec4 cout = vec4(0.0,0.0,0.0,0.0);"
"    float lum = color.x * 0.3 + color.y *0.59 + color.z * 0.11;"
"    vec4 p = color*(lum/AveLum);"
"    p /= vec4(vec4(1.0,1.0,1.0,0.0)+p);"
"    float luml = (p.x+p.y+p.z)/3.0;"
"    if (luml > 0.8)"
"    {"
"        cout = p;"
"    }"
"    gl_FragData[0] = cout;"
"}";

//Tone.fs
const CHAR* s_toneMappingPixelShaderString =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"uniform float AveLum;"
"uniform sampler2D gauss_texture;"
"uniform float Key;"
""
"const vec4 lumfact = vec4(0.27,0.67,0.06,0.0);"
""
"void main()"
"{"
"    vec4 color = texture2D(gauss_texture,v_tex0);"
"    float lum = dot(color , lumfact);"
"    color *= Key *lum/AveLum;"
"    color /= vec4(vec4(1.0,1.0,1.0,0.0)+color);"
"    gl_FragColor = clamp(color + texture2D(u_texture,v_tex0)*1.3, 0.0,1.0);"
"}";

#endif /* GraphicDisplayFilterCode_h */
