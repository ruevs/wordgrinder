#include "globals.h"
#include <GLFW/glfw3.h>

#define VKM_SHIFT 0x10000
#define VKM_CTRL 0x20000
#define VKM_CTRLASCII 0x40000
#define VK_RESIZE 0x80000
#define VK_TIMEOUT 0x80001
#define VK_QUIT 0x80002

struct cell
{
    uni_t c;
    uint8_t attr;
};

static GLFWwindow* window;
static int currentAttr;
static int screenWidth;
static int screenHeight;
static struct cell* screen;
static int cursorx;
static int cursory;
static bool cursorShown;

static int get_ivar(const char* name)
{
    lua_getglobal(L, "GlobalSettings");
    lua_getfield(L, -1, "gui");
    lua_getfield(L, -1, name);
    return luaL_checkinteger(L, -1);
}

static const char* get_svar(const char* name)
{
    lua_getglobal(L, "GlobalSettings");
    lua_getfield(L, -1, "gui");
    lua_getfield(L, -1, name);
    return lua_tostring(L, -1);
}

static void key_cb(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    printf("key %d %d %d %d\n", key, scancode, action, mods);
}

static void character_cb(GLFWwindow* window, unsigned int c)
{
    printf("char %d\n", c);
}

void dpy_init(const char* argv[]) {}

void dpy_start(void)
{
    if (!glfwInit())
    {
        fprintf(stderr, "OpenGL initialisation failed\n");
        exit(1);
    }

    window = glfwCreateWindow(get_ivar("window_width"),
        get_ivar("window_height"),
        "WordGrinder",
        NULL,
        NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_cb);
    glfwSetCharCallback(window, character_cb);
#if 0
    runOnUiThread(
        []
        {
            mainWindow = new wxFrame(nullptr,
                wxID_ANY,
                "WordGrinder",
                wxDefaultPosition,
                {getIvar("window_width"), getIvar("window_height")});

            mainWindow->SetIcon(createIcon());

            auto* sizer = new wxBoxSizer(wxHORIZONTAL);
            customView = new CustomView(mainWindow);
            sizer->Add(customView, 1, wxEXPAND);

            mainWindow->SetSizer(sizer);
            mainWindow->SetAutoLayout(true);

            mainWindow->Bind(wxEVT_CLOSE_WINDOW,
                [](auto& event)
                {
                    event.Veto();
                    pushKey(-VK_QUIT);
                });

            mainWindow->Show(true);
        });
#endif
}

void dpy_shutdown(void)
{
    glfwDestroyWindow(window);
    glfwTerminate();
#if 0
    runOnUiThread(
        []()
        {
            mainWindow->Close();
            flushFontCache();
        });
#endif
}

void dpy_clearscreen(void)
{
    dpy_cleararea(0, 0, screenWidth - 1, screenHeight - 1);
}

void dpy_getscreensize(int* x, int* y)
{
    *x = screenWidth;
    *y = screenHeight;
}

void dpy_sync(void)
{
    glfwSwapBuffers(window);
#if 0
    runOnUiThread(
        []()
        {
            customView->Sync();
        });
#endif
}

void dpy_setattr(int andmask, int ormask)
{
    currentAttr &= andmask;
    currentAttr |= ormask;
}

void dpy_writechar(int x, int y, uni_t c)
{
    if (!screen)
        return;
    if ((x < 0) || (x >= screenWidth))
        return;
    if ((y < 0) || (y >= screenHeight))
        return;

    struct cell* p = &screen[x + y * screenWidth];
    p->c = c;
    p->attr = currentAttr;
}

void dpy_cleararea(int x1, int y1, int x2, int y2)
{
    if (!screen)
        return;

    for (int y = y1; y <= y2; y++)
    {
        struct cell* p = &screen[y * screenWidth + x1];
        for (int x = x1; x <= x2; x++)
        {
            p->c = ' ';
            p->attr = currentAttr;
            p++;
        }
    }
}

void dpy_setcursor(int x, int y, bool shown)
{
    cursorx = x;
    cursory = y;
    cursorShown = shown;
}

uni_t dpy_getchar(double timeout)
{
    double endTime = glfwGetTime() + timeout;
    for (;;)
    {
        if (timeout == -1)
            glfwWaitEvents();
        else
        {
            double waitTime = endTime - glfwGetTime();
            if (waitTime < 0)
                return -VK_TIMEOUT;
            glfwWaitEventsTimeout(endTime);
        }
    }
#if 0
    auto endTime = wxGetLocalTimeMillis() + (timeout * 1000.0);
    for (;;)
    {
        /* Wait until the queue is non-empty. */

        if (timeout == -1)
            keyboardSemaphore.Wait();
        else
        {
            long delta = (endTime - wxGetLocalTimeMillis()).ToLong();
            if (delta <= 0)
                return -VK_TIMEOUT;
            keyboardSemaphore.WaitTimeout(delta);
        }

        /* If there's a key in the queue, pop it. */

        {
            wxMutexLocker m(keyboardMutex);
            if (!keyboardQueue.empty())
            {
                uni_t key = keyboardQueue.back();
                keyboardQueue.pop_back();
                return key;
            }
        }
    }
#endif
}

const char* dpy_getkeyname(uni_t k)
{
    return "KEY_TIMEOUT";
#if 0
    static char buffer[32];
    switch (-k)
    {
        case VK_RESIZE:
            return "KEY_RESIZE";
        case VK_TIMEOUT:
            return "KEY_TIMEOUT";
        case VK_QUIT:
            return "KEY_QUIT";
    }

    int mods = -k;
    int key = (-k & 0xfff0ffff);

    if (mods & VKM_CTRLASCII)
    {
        sprintf(buffer, "KEY_%s^%c", (mods & VKM_SHIFT) ? "S" : "", key + 64);
        return buffer;
    }

    const char* t = NULL;
    switch (key)
    {
        // clang-format off
        case WXK_DOWN:        t = "DOWN"; break;
        case WXK_UP:          t = "UP"; break;
        case WXK_LEFT:        t = "LEFT"; break;
        case WXK_RIGHT:       t = "RIGHT"; break;
        case WXK_HOME:        t = "HOME"; break;
        case WXK_END:         t = "END"; break;
        case WXK_BACK:        t = "BACKSPACE"; break;
        case WXK_DELETE:      t = "DELETE"; break;
        case WXK_INSERT:      t = "INSERT"; break;
        case WXK_PAGEUP:      t = "PGUP"; break;
        case WXK_PAGEDOWN:    t = "PGDN"; break;
        case WXK_TAB:         t = "TAB"; break;
        case WXK_RETURN:      t = "RETURN"; break;
        case WXK_ESCAPE:      t = "ESCAPE"; break;
        case WXK_MENU:        t = "MENU"; break;
            // clang-format on
    }

    if (t)
    {
        sprintf(buffer,
            "KEY_%s%s%s",
            (mods & VKM_SHIFT) ? "S" : "",
            (mods & VKM_CTRL) ? "^" : "",
            t);
        return buffer;
    }

    if ((key >= WXK_F1) && (key <= (WXK_F24)))
    {
        sprintf(buffer,
            "KEY_%s%sF%d",
            (mods & VKM_SHIFT) ? "S" : "",
            (mods & VKM_CTRL) ? "^" : "",
            key - WXK_F1 + 1);
        return buffer;
    }

    sprintf(buffer, "KEY_UNKNOWN_%d", -k);
    return buffer;
#endif
}

// vim: sw=4 ts=4 et
