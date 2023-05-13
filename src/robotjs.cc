#define NAPI_VERSION 4
#define NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED // https://github.com/nodejs/node-addon-api/blob/main/doc/external_buffer.md
#include <napi.h>
#include <vector>
#include "screen.h"
#include "screengrab.h"
#include "MMBitmap.h"
#include "snprintf.h"
#include "microsleep.h"
#if defined(USE_X11)
  #include "xdisplay.h"
#endif

void myDestroyMMBitmapBuffer(Napi::Env env, char * bitmapBuffer)
  {
    if (bitmapBuffer != NULL)
    {
      free(bitmapBuffer);
    }
  }

Napi::Value CaptureScreen(const Napi::CallbackInfo& info)
{
  size_t x;
  size_t y;
  size_t w;
  size_t h;

  //If user has provided screen coords, use them!
  if (info.Length() == 4)
  {
    // TODO: Make sure requested coords are within the screen bounds, or we get a seg fault.
    // An error message is much nicer!

    x = info[0].As<Napi::Number>().ToNumber().Int32Value();
    y = info[1].As<Napi::Number>().ToNumber().Int32Value();
    w = info[2].As<Napi::Number>().ToNumber().Int32Value();
    h = info[3].As<Napi::Number>().ToNumber().Int32Value();
  }
  else
  {
    //We're getting the full screen.
    x = 0;
    y = 0;

    //Get screen size.
    MMSize displaySize = getMainDisplaySize();
    w = displaySize.width;
    h = displaySize.height;
  }

  MMBitmapRef bitmap = copyMMBitmapFromDisplayInRect(MMRectMake(x, y, w, h));

  uint32_t bufferSize = bitmap->bytewidth * bitmap->height;
  auto buffer = Napi::Buffer<char>::NewOrCopy(info.Env(), (char*)bitmap->imageBuffer, bufferSize, myDestroyMMBitmapBuffer);

  Napi::Object obj = Napi::Object::New(info.Env());
  obj.Set("width", bitmap->width);
  obj.Set("height", bitmap->height);
  obj.Set("byteWidth", bitmap->bytewidth);
  obj.Set("bitsPerPixel", bitmap->bitsPerPixel);
  obj.Set("bytesPerPixel", bitmap->bytesPerPixel);
  obj.Set("image", buffer);

  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "captureScreen"),
              Napi::Function::New(env, CaptureScreen));

  return exports;
}

NODE_API_MODULE(robotjs, Init)