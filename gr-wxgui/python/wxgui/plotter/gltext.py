#!/usr/bin/env python
# -*- coding: utf-8
#
#    Provides some text display functions for wx + ogl
#    Copyright (C) 2007 Christian Brugger, Stefan Hacker
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import wx
from OpenGL.GL import *

"""
Optimize with psyco if possible, this gains us about 50% speed when
creating our textures in trade for about 4MBytes of additional memory usage for
psyco. If you don't like losing the memory you have to turn the lines following
"enable psyco" into a comment while uncommenting the line after "Disable psyco".
"""
#Try to enable psyco
try:
    import psyco
    psyco_optimized = False
except ImportError:
    psyco = None

#Disable psyco
#psyco = None

class TextElement(object):
    """
    A simple class for using system Fonts to display
    text in an OpenGL scene
    """
    def __init__(self,
                 text = '',
                 font = None,
                 foreground = wx.BLACK,
                 centered = False):
        """
        text (String)         - Text
        font (wx.Font)        - Font to draw with (None = System default)
        foreground (wx.Color) - Color of the text
                or (wx.Bitmap)- Bitmap to overlay the text with
        centered (bool)       - Center the text

        Initializes the TextElement
        """
        # save given variables
        self._text        = text
        self._lines       = text.split('\n')
        self._font        = font
        self._foreground  = foreground
        self._centered    = centered

        # init own variables
        self._owner_cnt   = 0        #refcounter
        self._texture     = None     #OpenGL texture ID
        self._text_size   = None     #x/y size tuple of the text
        self._texture_size= None     #x/y Texture size tuple

        # create Texture
        self.createTexture()


    #---Internal helpers

    def _getUpper2Base(self, value):
        """
        Returns the lowest value with the power of
        2 greater than 'value' (2^n>value)
        """
        base2 = 1
        while base2 < value:
            base2 *= 2
        return base2

    #---Functions

    def draw_text(self, position = wx.Point(0,0), scale = 1.0, rotation = 0):
        """
        position (wx.Point)    - x/y Position to draw in scene
        scale    (float)       - Scale
        rotation (int)         - Rotation in degree

        Draws the text to the scene
        """
        #Enable necessary functions
        glColor(1,1,1,1)
        glEnable(GL_TEXTURE_2D)
        glEnable(GL_ALPHA_TEST)       #Enable alpha test
        glAlphaFunc(GL_GREATER, 0)
        glEnable(GL_BLEND)            #Enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        #Bind texture
        glBindTexture(GL_TEXTURE_2D, self._texture)

        ow, oh = self._text_size
        w , h  = self._texture_size
        #Perform transformations
        glPushMatrix()
        glTranslated(position.x, position.y, 0)
        glRotate(-rotation, 0, 0, 1)
        glScaled(scale, scale, scale)
        if self._centered:
            glTranslate(-w/2, -oh/2, 0)
        #Draw vertices
        glBegin(GL_QUADS)
        glTexCoord2f(0,0); glVertex2f(0,0)
        glTexCoord2f(0,1); glVertex2f(0,h)
        glTexCoord2f(1,1); glVertex2f(w,h)
        glTexCoord2f(1,0); glVertex2f(w,0)
        glEnd()
        glPopMatrix()

        #Disable features
        glDisable(GL_BLEND)
        glDisable(GL_ALPHA_TEST)
        glDisable(GL_TEXTURE_2D)

    def createTexture(self):
        """
        Creates a texture from the settings saved in TextElement, to be able to use normal
        system fonts conviently a wx.MemoryDC is used to draw on a wx.Bitmap. As wxwidgets
        device contexts don't support alpha at all it is necessary to apply a little hack
        to preserve antialiasing without sticking to a fixed background color:

        We draw the bmp in b/w mode so we can use its data as a alpha channel for a solid
        color bitmap which after GL_ALPHA_TEST and GL_BLEND will show a nicely antialiased
        text on any surface.

        To access the raw pixel data the bmp gets converted to a wx.Image. Now we just have
        to merge our foreground color with the alpha data we just created and push it all
        into a OpenGL texture and we are DONE *inhalesdelpy*

        DRAWBACK of the whole conversion thing is a really long time for creating the
        texture. If you see any optimizations that could save time PLEASE CREATE A PATCH!!!
        """
        # get a memory dc
        dc = wx.MemoryDC()

        # Select an empty bitmap into the MemoryDC - otherwise the call to
        # GetMultiLineTextExtent() may fail below
        dc.SelectObject(wx.EmptyBitmap(1,1))

        # set our font
        dc.SetFont(self._font)

        # Approximate extend to next power of 2 and create our bitmap
        # REMARK: You wouldn't believe how much fucking speed this little
        #         sucker gains compared to sizes not of the power of 2. It's like
        #         500ms --> 0.5ms (on my ATI-GPU powered Notebook). On Sams nvidia
        #         machine there don't seem to occur any losses...bad drivers?
        ow, oh = dc.GetMultiLineTextExtent(self._text)[:2]
        w, h = self._getUpper2Base(ow), self._getUpper2Base(oh)

        self._text_size = wx.Size(ow,oh)
        self._texture_size = wx.Size(w,h)
        bmp = wx.EmptyBitmap(w,h)


        #Draw in b/w mode to bmp so we can use it as alpha channel
        dc.SelectObject(bmp)
        dc.SetBackground(wx.BLACK_BRUSH)
        dc.Clear()
        dc.SetTextForeground(wx.WHITE)
        x,y = 0,0
        centered = self.centered
        for line in self._lines:
            if not line: line = ' '
            tw, th = dc.GetTextExtent(line)
            if centered:
                x = int(round((w-tw)/2))
            dc.DrawText(line, x, y)
            x = 0
            y += th
        #Release the dc
        dc.SelectObject(wx.NullBitmap)
        del dc

        #Generate a correct RGBA data string from our bmp
        """
        NOTE: You could also use wx.AlphaPixelData to access the pixel data
        in 'bmp' directly, but the iterator given by it is much slower than
        first converting to an image and using wx.Image.GetData().
        """
        img   = wx.ImageFromBitmap(bmp)
        alpha = img.GetData()

        if isinstance(self._foreground, wx.Colour):
            """
            If we have a static color...
            """
            r,g,b = self._foreground.Get()
            color = "%c%c%c" % (chr(r), chr(g), chr(b))

            data = ''
            for i in xrange(0, len(alpha)-1, 3):
                data += color + alpha[i]

        elif isinstance(self._foreground, wx.Bitmap):
            """
            If we have a bitmap...
            """
            bg_img    = wx.ImageFromBitmap(self._foreground)
            bg        = bg_img.GetData()
            bg_width  = self._foreground.GetWidth()
            bg_height = self._foreground.GetHeight()

            data = ''

            for y in xrange(0, h):
                for x in xrange(0, w):
                    if (y > (bg_height-1)) or (x > (bg_width-1)):
                        color = "%c%c%c" % (chr(0),chr(0),chr(0))
                    else:
                        pos = (x+y*bg_width) * 3
                        color = bg[pos:pos+3]
                    data += color + alpha[(x+y*w)*3]


        # now convert it to ogl texture
        self._texture = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, self._texture)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 2)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data)

    def deleteTexture(self):
        """
        Deletes the OpenGL texture object
        """
        if self._texture:
            if glIsTexture(self._texture):
                glDeleteTextures(self._texture)
            else:
                self._texture = None

    def bind(self):
        """
        Increase refcount
        """
        self._owner_cnt += 1

    def release(self):
        """
        Decrease refcount
        """
        self._owner_cnt -= 1

    def isBound(self):
        """
        Return refcount
        """
        return self._owner_cnt

    def __del__(self):
        """
        Destructor
        """
        self.deleteTexture()

    #---Getters/Setters

    def getText(self): return self._text
    def getFont(self): return self._font
    def getForeground(self): return self._foreground
    def getCentered(self): return self._centered
    def getTexture(self): return self._texture
    def getTexture_size(self): return self._texture_size

    def getOwner_cnt(self): return self._owner_cnt
    def setOwner_cnt(self, value):
        self._owner_cnt = value

    #---Properties

    text         = property(getText, None, None, "Text of the object")
    font         = property(getFont, None, None, "Font of the object")
    foreground   = property(getForeground, None, None, "Color of the text")
    centered     = property(getCentered, None, None, "Is text centered")
    owner_cnt    = property(getOwner_cnt, setOwner_cnt, None, "Owner count")
    texture      = property(getTexture, None, None, "Used texture")
    texture_size = property(getTexture_size, None, None, "Size of the used texture")


class Text(object):
    """
    A simple class for using System Fonts to display text in
    an OpenGL scene. The Text adds a global Cache of already
    created text elements to TextElement's base functionality
    so you can save some memory and increase speed
    """
    _texts         = []    #Global cache for TextElements

    def __init__(self,
                 text = 'Text',
                 font = None,
                 font_size = 8,
                 foreground = wx.BLACK,
                 centered = False,
                 bold = False):
        """
            text (string)           - displayed text
            font (wx.Font)          - if None, system default font will be used with font_size
            font_size (int)         - font size in points
            foreground (wx.Color)   - Color of the text
                    or (wx.Bitmap)  - Bitmap to overlay the text with
            centered (bool)         - should the text drawn centered towards position?

            Initializes the text object
        """
        #Init/save variables
        self._aloc_text = None
        self._text      = text
        self._font_size = font_size
        self._foreground= foreground
        self._centered  = centered

        #Check if we are offered a font
        if not font:
            #if not use the system default
            self._font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        else:
            #save it
            self._font = font

        if bold: self._font.SetWeight(wx.FONTWEIGHT_BOLD)

        #Bind us to our texture
        self._initText()

    #---Internal helpers

    def _initText(self):
        """
        Initializes/Reinitializes the Text object by binding it
        to a TextElement suitable for its current settings
        """
        #Check if we already bound to a texture
        if self._aloc_text:
            #if so release it
            self._aloc_text.release()
            if not self._aloc_text.isBound():
                self._texts.remove(self._aloc_text)
            self._aloc_text = None

        #Adjust our font
        self._font.SetPointSize(self._font_size)

        #Search for existing element in our global buffer
        for element in self._texts:
            if element.text == self._text and\
              element.font == self._font and\
              element.foreground == self._foreground and\
              element.centered == self._centered:
                # We already exist in global buffer ;-)
                element.bind()
                self._aloc_text = element
                break

        if not self._aloc_text:
            # We are not in the global buffer, let's create ourselves
            aloc_text = self._aloc_text = TextElement(self._text,
                                                       self._font,
                                                       self._foreground,
                                                       self._centered)
            aloc_text.bind()
            self._texts.append(aloc_text)

    def __del__(self):
        """
        Destructor
        """
        aloc_text = self._aloc_text
        aloc_text.release()
        if not aloc_text.isBound():
            self._texts.remove(aloc_text)

    #---Functions

    def draw_text(self, position = wx.Point(0,0), scale = 1.0, rotation = 0):
        """
        position (wx.Point)    - x/y Position to draw in scene
        scale    (float)       - Scale
        rotation (int)         - Rotation in degree

        Draws the text to the scene
        """

        self._aloc_text.draw_text(position, scale, rotation)

    #---Setter/Getter

    def getText(self): return self._text
    def setText(self, value, reinit = True):
        """
        value (bool)    - New Text
        reinit (bool)   - Create a new texture

        Sets a new text
        """
        self._text = value
        if reinit:
            self._initText()

    def getFont(self): return self._font
    def setFont(self, value, reinit = True):
        """
        value (bool)    - New Font
        reinit (bool)   - Create a new texture

        Sets a new font
        """
        self._font = value
        if reinit:
            self._initText()

    def getFont_size(self): return self._font_size
    def setFont_size(self, value, reinit = True):
        """
        value (bool)    - New font size
        reinit (bool)   - Create a new texture

        Sets a new font size
        """
        self._font_size = value
        if reinit:
            self._initText()

    def getForeground(self): return self._foreground
    def setForeground(self, value, reinit = True):
        """
        value (bool)    - New centered value
        reinit (bool)   - Create a new texture

        Sets a new value for 'centered'
        """
        self._foreground = value
        if reinit:
            self._initText()

    def getCentered(self): return self._centered
    def setCentered(self, value, reinit = True):
        """
        value (bool)    - New centered value
        reinit (bool)   - Create a new texture

        Sets a new value for 'centered'
        """
        self._centered = value
        if reinit:
            self._initText()

    def get_size(self):
        """
        Returns a text size tuple
        """
        return self._aloc_text._text_size

    def getTexture_size(self):
        """
        Returns a texture size tuple
        """
        return self._aloc_text.texture_size

    def getTextElement(self):
        """
        Returns the text element bound to the Text class
        """
        return self._aloc_text

    def getTexture(self):
        """
        Returns the texture of the bound TextElement
        """
        return self._aloc_text.texture


    #---Properties

    text         = property(getText, setText, None, "Text of the object")
    font         = property(getFont, setFont, None, "Font of the object")
    font_size    = property(getFont_size, setFont_size, None, "Font size")
    foreground   = property(getForeground, setForeground, None, "Color/Overlay bitmap of the text")
    centered     = property(getCentered, setCentered, None, "Display the text centered")
    texture_size = property(getTexture_size, None, None, "Size of the used texture")
    texture      = property(getTexture, None, None, "Texture of bound TextElement")
    text_element = property(getTextElement,None , None, "TextElement bound to this class")

#Optimize critical functions
if psyco and not psyco_optimized:
    psyco.bind(TextElement.createTexture)
    psyco_optimized = True
