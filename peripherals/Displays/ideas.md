Looking back at the progression, there are really five distinct architectures that emerged. They differ mainly in where pixels live, who owns composition, and how responsive layouts would work. Your ideas gradually moved from "windowed framebuffer" toward "fake GPU with a compositor", while my suggestions explored alternatives used by historical systems and embedded GUIs.

# Comparison at a glance

|
Architecture

|

Pixel ownership

|

RAM cost

|

Best trait

|

Biggest drawback

|
| --- | --- | --- | --- | --- |
|

1. Per-window framebuffer

|

Each window

|

Very high

|

Independent windows

|

Doesn't scale on RP2350

|
|

2. Single shared framebuffer

|

One global FB

|

150 KB

|

Simple composition

|

Full FB always occupied

|
|

3. Scratch-tile compositor

|

Small tile only

|

Very low

|

Excellent memory usage

|

More complex redraw logic

|
|

4. Immediate-mode GPU (your latest)

|

None

|

Very low

|

Apps only issue commands

|

Needs command scheduling

|
|

5. Retained display lists

|

Command lists

|

Low

|

Easy redraw of exposed regions

|

Command lifetime management

|

# 1. Per-window framebuffer (initial assumption)

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20640%20220%22%20width%3D%22100%25%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%2230%22%20y%3D%2240%22%20width%3D%22150%22%20height%3D%22120%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22105%22%20y%3D%22105%22%20text-anchor%3D%22middle%22%20font-size%3D%2214%22%3EWindow%20A%20FB%3C%2Ftext%3E%3Crect%20x%3D%22245%22%20y%3D%2260%22%20width%3D%22150%22%20height%3D%22120%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22320%22%20y%3D%22125%22%20text-anchor%3D%22middle%22%20font-size%3D%2214%22%3EWindow%20B%20FB%3C%2Ftext%3E%3Crect%20x%3D%22460%22%20y%3D%2230%22%20width%3D%22150%22%20height%3D%22150%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22535%22%20y%3D%22105%22%20text-anchor%3D%22middle%22%20font-size%3D%2214%22%3ECompose%3C%2Ftext%3E%3C%2Fsvg%3E)

This was my first assumption.

### Flow

* Every app requests a surface.

* It draws into its own buffer.

* Compositor merges them by Z-order.

### Pros

* Windows redraw independently.

* Overlap is easy.

### Cons

A single fullscreen RGB565 buffer is already:

240×320×2≈150 KB240\times320\times2\approx150\text{ KB}240×320×2≈150 KB

Three fullscreen windows would consume nearly the RP2350's available SRAM.

Verdict: Not suitable.

# 2. Single shared framebuffer (your correction)

This was your first important correction.

Instead of multiple buffers:

* One framebuffer exists.

* Windows are metadata only.

* Apps draw into the shared surface.

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20640%20220%22%20width%3D%22100%25%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%22170%22%20y%3D%2235%22%20width%3D%22300%22%20height%3D%22150%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Ctext%20x%3D%22320%22%20y%3D%2260%22%20text-anchor%3D%22middle%22%20font-size%3D%2216%22%3ESingle%20Framebuffer%3C%2Ftext%3E%3Crect%20x%3D%22210%22%20y%3D%2280%22%20width%3D%22120%22%20height%3D%2260%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22270%22%20y%3D%22115%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EWindow%20A%3C%2Ftext%3E%3Crect%20x%3D%22290%22%20y%3D%22105%22%20width%3D%22120%22%20height%3D%2260%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22350%22%20y%3D%22140%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EWindow%20B%3C%2Ftext%3E%3C%2Fsvg%3E)

### Flow

* Windows have position.

* Compositor clips drawing.

* Lowest Z draws first.

### Responsive layout

This introduced the idea that rotation changes window placement rather than rotating window contents.

### Pros

* Memory-efficient.

* Easy DMA.

### Cons

* Apps still effectively write pixels.

# 3. Scratch-tile compositor (future optimization)

This was my "old graphics systems" suggestion.

Instead of storing a permanent framebuffer:

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20700%20220%22%20width%3D%22100%25%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%2240%22%20y%3D%2250%22%20width%3D%22120%22%20height%3D%2290%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22100%22%20y%3D%22100%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EWindow%3C%2Ftext%3E%3Cpath%20d%3D%22M160%2095%20L280%2095%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Cpolygon%20points%3D%22280%2C95%20268%2C89%20268%2C101%22%20fill%3D%22currentColor%22%2F%3E%3Crect%20x%3D%22280%22%20y%3D%2255%22%20width%3D%22140%22%20height%3D%2280%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22350%22%20y%3D%2295%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EScratch%20Tile%3C%2Ftext%3E%3Cpath%20d%3D%22M420%2095%20L540%2095%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Cpolygon%20points%3D%22540%2C95%20528%2C89%20528%2C101%22%20fill%3D%22currentColor%22%2F%3E%3Crect%20x%3D%22540%22%20y%3D%2240%22%20width%3D%22120%22%20height%3D%22110%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22600%22%20y%3D%22100%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EILI9341%3C%2Ftext%3E%3C%2Fsvg%3E)

* Render only a strip (for example, 240×16).

* DMA immediately.

* Repeat.

This resembles:

* Amiga region rendering

* X11 damage rendering

* Modern LVGL partial buffering

### Pros

* Extremely low RAM.

* Good for SPI displays.

### Cons

* Harder scheduling.

I don't think this should be your first implementation, but your API should allow migrating to it later.

# 4. Fake GPU / Immediate-mode GPU (your latest idea)

This is where your design became much more distinct.

Instead of apps touching pixels:

They submit commands.

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20760%20260%22%20width%3D%22100%25%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%2230%22%20y%3D%2240%22%20width%3D%22120%22%20height%3D%2250%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%2290%22%20y%3D%2270%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EApp%3C%2Ftext%3E%3Cpath%20d%3D%22M150%2065%20L280%2065%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Cpolygon%20points%3D%22280%2C65%20268%2C59%20268%2C71%22%20fill%3D%22currentColor%22%2F%3E%3Crect%20x%3D%22280%22%20y%3D%2225%22%20width%3D%22180%22%20height%3D%22120%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22370%22%20y%3D%2250%22%20text-anchor%3D%22middle%22%20font-size%3D%2214%22%3ECommand%20Queue%3C%2Ftext%3E%3Ctext%20x%3D%22370%22%20y%3D%2275%22%20text-anchor%3D%22middle%22%20font-size%3D%2212%22%3EFillRect%3C%2Ftext%3E%3Ctext%20x%3D%22370%22%20y%3D%2295%22%20text-anchor%3D%22middle%22%20font-size%3D%2212%22%3EText%3C%2Ftext%3E%3Ctext%20x%3D%22370%22%20y%3D%22115%22%20text-anchor%3D%22middle%22%20font-size%3D%2212%22%3EBlit%3C%2Ftext%3E%3Cpath%20d%3D%22M460%2085%20L590%2085%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Cpolygon%20points%3D%22590%2C85%20578%2C79%20578%2C91%22%20fill%3D%22currentColor%22%2F%3E%3Crect%20x%3D%22590%22%20y%3D%2240%22%20width%3D%22140%22%20height%3D%2290%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22660%22%20y%3D%2275%22%20text-anchor%3D%22middle%22%20font-size%3D%2214%22%3EILI9341%3C%2Ftext%3E%3C%2Fsvg%3E)

Example:

C++

```
ctx.fillRect(...);
ctx.drawText(...);
ctx.blit(...);
```

### Internally

Those become packets.

C++

```
FillRect {
    windowID;
    x;
    y;
    w;
    h;
    color;
}
```

Notice:

* coordinates remain local

* no framebuffer ownership

* compositor translates later

This resembles:

* Amiga Blitter command submission

* GDI drawing commands

* OpenGL immediate mode

### Why this fits RP2350

Most GUI operations are actually:

* rectangles

* text

* icons

* lines

Those are compact commands compared to pixel buffers.

This is probably the strongest direction so far.

# 5. Retained display lists (my refinement)

I suggested extending your command idea.

Instead of discarding commands:

Each window keeps a command list.

C++

```
commands[] = {
    FillRect(...),
    DrawText(...),
    Blit(...)
};
```

When another window moves away:

* replay affected commands.

This resembles:

* retained-mode GUIs

* scene graphs

* Dear ImGui draw lists (rebuilt frequently)

### Problem

Dynamic content.

Example:

```
12:34
```

becomes

```
12:35
```

Now old commands exist too.

That means command lifetime becomes another memory management problem.

# Responsive layout evolution

This part actually became fairly consistent.

## Stage 1

Absolute coordinates.

```
x=10
y=20
```

Rotation breaks layout.

## Stage 2

Anchor-based.

```
Bottom
Center
Fill
```

Better.

## Stage 3

Constraint-based.

```
left=8
right=8
bottom=8
height=80
```

Portrait

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20260%20340%22%20width%3D%22180%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%2220%22%20y%3D%2220%22%20width%3D%22220%22%20height%3D%22300%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Crect%20x%3D%2228%22%20y%3D%22232%22%20width%3D%22204%22%20height%3D%2280%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22130%22%20y%3D%22275%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EWindow%3C%2Ftext%3E%3C%2Fsvg%3E)

Landscape

![](data\:image/svg+xml;charset=utf-8,%3Csvg%20font-family%3D%22-apple-system-body%2C%20ui-sans-serif%2C%20-apple-system%2C%20system-ui%2C%20%26quot%3BSegoe%20UI%26quot%3B%2C%20Helvetica%2C%20%26quot%3BApple%20Color%20Emoji%26quot%3B%2C%20Arial%2C%20sans-serif%2C%20%26quot%3BSegoe%20UI%20Emoji%26quot%3B%2C%20%26quot%3BSegoe%20UI%20Symbol%26quot%3B%22%20font-weight%3D%22400%22%20data-d-component%3D%22svg%22%20fill%3D%22currentColor%22%20style%3D%22color%3Argb\(255%2C%20255%2C%20255\)%22%20viewBox%3D%220%200%20340%20260%22%20width%3D%22220%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%3E%3Crect%20x%3D%2220%22%20y%3D%2220%22%20width%3D%22300%22%20height%3D%22220%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%20stroke-width%3D%222%22%2F%3E%3Crect%20x%3D%2228%22%20y%3D%22152%22%20width%3D%22284%22%20height%3D%2280%22%20fill%3D%22none%22%20stroke%3D%22currentColor%22%2F%3E%3Ctext%20x%3D%22170%22%20y%3D%22195%22%20text-anchor%3D%22middle%22%20font-size%3D%2213%22%3EWindow%3C%2Ftext%3E%3C%2Fsvg%3E)

The same constraints automatically produce both layouts.

I think this remains the right approach regardless of the rendering backend.

# Historical parallels

Interestingly, each idea maps closely to an older graphics system.

|
Your/Our Design

|

Historical analogue

|
| --- | --- |
|

Per-window framebuffer

|

Modern desktop compositors

|
|

Shared framebuffer

|

Early framebuffer GUIs

|
|

Scratch tile

|

Amiga/X11 damage rendering

|
|

Fake GPU command queue

|

Amiga Blitter, Windows GDI

|
|

Retained display list

|

Scene graphs, retained GUIs

|

# Where I think your design is converging

The newest idea is no longer really a "window manager."

It's becoming a 2D graphics server.

Applications don't receive memory.

They receive a graphics context.

```
ctx.fillRect(...)
ctx.drawText(...)
ctx.blit(...)
```

Internally:

1. Commands stay in window-local coordinates.

2. Layout computes window positions for the current orientation.

3. Compositor clips against visible regions and Z-order.

4. Rasterizer executes commands into the shared framebuffer (or a future scratch tile).

5. DMA pushes only the dirty regions to the ILI9341.

That separation is particularly valuable because it preserves the application-facing API while leaving the rendering backend free to evolve from a permanent 150 KB framebuffer to a tiled renderer later without changing how applications draw.
