#
# Kaffe AWT implementation.
#
# Copyright (c) 2004
# 	Kaffe.org contributors. All rights reserved. See ChageLog for details.
#
# See the file "license.terms" for information on usage and redistribution
# of this file.

CLASSFILE = kaffeawt.jar
SRCDIRS = kaffe java

jrelib_DATA = $(CLASSFILE)

kaffeawt_jar_SRCS = \
	$(java_applet_SRCS) \
	$(java_awt_SRCS) \
	$(java_awt_color_SRCS) \
	$(java_awt_datatransfer_SRCS) \
	$(java_awt_dnd_SRCS) \
	$(java_awt_dnd_peer_SRCS) \
	$(java_awt_event_SRCS) \
	$(java_awt_font_SRCS) \
	$(java_awt_geom_SRCS) \
	$(java_awt_im_SRCS) \
	$(java_awt_image_SRCS) \
	$(java_awt_image_renderable_SRCS) \
	$(java_awt_peer_SRCS) \
	$(java_awt_print_SRCS) \
	$(kaffe_applet_SRCS) \
	$(kaffe_awt_SRCS) \
        $(kaffe_util_log_SRCS)
java_applet_SRCS = \
	java/applet/AppletContext.java \
	java/applet/Applet.java \
	java/applet/AppletStub.java \
	java/applet/AudioClip.java
java_awt_SRCS = \
	java/awt/ActionEvt.java \
	java/awt/ActiveEvent.java \
	java/awt/Adjustable.java \
	java/awt/AdjustmentEvt.java \
	java/awt/AlphaComposite.java \
	java/awt/AttributeValue.java \
	java/awt/AWTError.java \
	java/awt/AWTEvent.java \
	java/awt/AWTEventMulticaster.java \
	java/awt/AWTException.java \
	java/awt/AWTPermission.java \
	java/awt/BarMenu.java \
	java/awt/BasicStroke.java \
	java/awt/BorderLayout.java \
	java/awt/BufferCapabilities.java \
	java/awt/Button.java \
	java/awt/Canvas.java \
	java/awt/CardLayout.java \
	java/awt/CheckboxGroup.java \
	java/awt/Checkbox.java \
	java/awt/CheckboxMenuItem.java \
	java/awt/Choice.java \
	java/awt/ClassAnalyzer.java \
	java/awt/ClassProperties.java \
	java/awt/Color.java \
	java/awt/ColorPaintContext.java \
	java/awt/ComponentEvt.java \
	java/awt/Component.java \
	java/awt/ComponentOrientation.java \
	java/awt/CompositeContext.java \
	java/awt/Composite.java \
	java/awt/ContainerEvt.java \
	java/awt/Container.java \
	java/awt/Cursor.java \
	java/awt/Defaults.java \
	java/awt/DefKeyFilter.java \
	java/awt/Dialog.java \
	java/awt/Dimension.java \
	java/awt/DisplayMode.java \
	java/awt/EventDispatchThread.java \
	java/awt/Event.java \
	java/awt/EventQueue.java \
	java/awt/FileDialog.java \
	java/awt/FlowLayout.java \
	java/awt/FocusEvt.java \
	java/awt/FocusTraversalPolicy.java \
	java/awt/FontFormatException.java \
	java/awt/Font.java \
	java/awt/FontMetrics.java \
	java/awt/Frame.java \
	java/awt/GradientPaint.java \
	java/awt/Graphics2D.java \
	java/awt/GraphicsConfigTemplate.java \
	java/awt/GraphicsConfiguration.java \
	java/awt/GraphicsDevice.java \
	java/awt/GraphicsEnvironment.java \
	java/awt/Graphics.java \
	java/awt/GraphicsLink.java \
	java/awt/GridBagConstraints.java \
	java/awt/GridBagLayoutInfo.java \
	java/awt/GridBagLayout.java \
	java/awt/GridLayout.java \
	java/awt/HeadlessException.java \
	java/awt/IllegalComponentStateException.java \
	java/awt/ImageCapabilities.java \
	java/awt/ImageFrameLoader.java \
	java/awt/Image.java \
	java/awt/ImageLoader.java \
	java/awt/ImageNativeProducer.java \
	java/awt/Insets.java \
	java/awt/ItemEvt.java \
	java/awt/ItemSelectable.java \
	java/awt/JobAttributes.java \
	java/awt/KeyEventDispatcher.java \
	java/awt/KeyEventPostProcessor.java \
	java/awt/KeyEvt.java \
	java/awt/Label.java \
	java/awt/LayoutManager2.java \
	java/awt/LayoutManager.java \
	java/awt/List.java \
	java/awt/MediaTracker.java \
	java/awt/MenuBar.java \
	java/awt/MenuComponent.java \
	java/awt/MenuContainer.java \
	java/awt/MenuItem.java \
	java/awt/Menu.java \
	java/awt/MenuShortcut.java \
	java/awt/MouseEvt.java \
	java/awt/NativeClipboard.java \
	java/awt/NativeGraphics.java \
	java/awt/NativeSelection.java \
	java/awt/OpaqueComponent.java \
	java/awt/PageAttributes.java \
	java/awt/PaintContext.java \
	java/awt/PaintEvt.java \
	java/awt/Paint.java \
	java/awt/Panel.java \
	java/awt/Point.java \
	java/awt/Polygon.java \
	java/awt/PopupMenu.java \
	java/awt/PopupWindow.java \
	java/awt/PrintGraphics.java \
	java/awt/PrintJob.java \
	java/awt/PSGraphics.java \
	java/awt/PSPrintJob.java \
	java/awt/Rectangle.java \
	java/awt/RenderingHints.java \
	java/awt/Robot.java \
	java/awt/RootWindow.java \
	java/awt/RowCanvas.java \
	java/awt/Scrollbar.java \
	java/awt/ScrollPaneAdjustable.java \
	java/awt/ScrollPane.java \
	java/awt/Shape.java \
	java/awt/ShortcutConsumer.java \
	java/awt/ShortcutHandler.java \
	java/awt/Stroke.java \
	java/awt/SystemColor.java \
	java/awt/TextArea.java \
	java/awt/TextBuffer.java \
	java/awt/TextComponent.java \
	java/awt/TextCursor.java \
	java/awt/TextEvt.java \
	java/awt/TextField.java \
	java/awt/TexturePaint.java \
	java/awt/Toolkit.java \
	java/awt/Transparency.java \
	java/awt/WindowEvt.java \
	java/awt/Window.java \
	java/awt/WMEvent.java
java_awt_color_SRCS = \
	java/awt/color/CMMException.java \
	java/awt/color/ColorSpace.java \
	java/awt/color/ICC_ColorSpace.java \
	java/awt/color/ICC_ProfileGray.java \
	java/awt/color/ICC_Profile.java \
	java/awt/color/ICC_ProfileRGB.java \
	java/awt/color/ProfileDataException.java
java_awt_datatransfer_SRCS = \
	java/awt/datatransfer/Clipboard.java \
	java/awt/datatransfer/ClipboardOwner.java \
	java/awt/datatransfer/DataFlavor.java \
	java/awt/datatransfer/FlavorMap.java \
	java/awt/datatransfer/FlavorTable.java \
	java/awt/datatransfer/MimeTypeParseException.java \
	java/awt/datatransfer/StringSelection.java \
	java/awt/datatransfer/SystemFlavorMap.java \
	java/awt/datatransfer/Transferable.java \
	java/awt/datatransfer/UnsupportedFlavorException.java
java_awt_dnd_SRCS = \
	java/awt/dnd/Autoscroll.java \
	java/awt/dnd/DnDConstants.java \
	java/awt/dnd/DnDEventMulticaster.java \
	java/awt/dnd/DragGestureEvent.java \
	java/awt/dnd/DragGestureListener.java \
	java/awt/dnd/DragGestureRecognizer.java \
	java/awt/dnd/DragSourceAdapter.java \
	java/awt/dnd/DragSourceContext.java \
	java/awt/dnd/DragSourceDragEvent.java \
	java/awt/dnd/DragSourceDropEvent.java \
	java/awt/dnd/DragSourceEvent.java \
	java/awt/dnd/DragSource.java \
	java/awt/dnd/DragSourceListener.java \
	java/awt/dnd/DragSourceMotionListener.java \
	java/awt/dnd/DropTargetAdapter.java \
	java/awt/dnd/DropTargetContext.java \
	java/awt/dnd/DropTargetDragEvent.java \
	java/awt/dnd/DropTargetDropEvent.java \
	java/awt/dnd/DropTargetEvent.java \
	java/awt/dnd/DropTarget.java \
	java/awt/dnd/DropTargetListener.java \
	java/awt/dnd/InvalidDnDOperationException.java \
	java/awt/dnd/MouseDragGestureRecognizer.java
java_awt_dnd_peer_SRCS = \
	java/awt/dnd/peer/DragSourceContextPeer.java \
	java/awt/dnd/peer/DropTargetContextPeer.java \
	java/awt/dnd/peer/DropTargetPeer.java
java_awt_event_SRCS = \
	java/awt/event/ActionEvent.java \
	java/awt/event/ActionListener.java \
	java/awt/event/AdjustmentEvent.java \
	java/awt/event/AdjustmentListener.java \
	java/awt/event/AWTEventListener.java \
	java/awt/event/ComponentAdapter.java \
	java/awt/event/ComponentEvent.java \
	java/awt/event/ComponentListener.java \
	java/awt/event/ContainerAdapter.java \
	java/awt/event/ContainerEvent.java \
	java/awt/event/ContainerListener.java \
	java/awt/event/FocusAdapter.java \
	java/awt/event/FocusEvent.java \
	java/awt/event/FocusListener.java \
	java/awt/event/HierarchyBoundsAdapter.java \
	java/awt/event/HierarchyBoundsListener.java \
	java/awt/event/HierarchyEvent.java \
	java/awt/event/HierarchyListener.java \
	java/awt/event/InputEvent.java \
	java/awt/event/InputMethodEvent.java \
	java/awt/event/InputMethodListener.java \
	java/awt/event/InvocationEvent.java \
	java/awt/event/ItemEvent.java \
	java/awt/event/ItemListener.java \
	java/awt/event/KeyAdapter.java \
	java/awt/event/KeyEvent.java \
	java/awt/event/KeyListener.java \
	java/awt/event/MouseAdapter.java \
	java/awt/event/MouseEvent.java \
	java/awt/event/MouseListener.java \
	java/awt/event/MouseMotionAdapter.java \
	java/awt/event/MouseMotionListener.java \
	java/awt/event/MouseWheelEvent.java \
	java/awt/event/MouseWheelListener.java \
	java/awt/event/PaintEvent.java \
	java/awt/event/TextEvent.java \
	java/awt/event/TextListener.java \
	java/awt/event/WindowAdapter.java \
	java/awt/event/WindowEvent.java \
	java/awt/event/WindowFocusListener.java \
	java/awt/event/WindowListener.java \
	java/awt/event/WindowStateListener.java
java_awt_font_SRCS = \
	java/awt/font/FontRenderContext.java \
	java/awt/font/GlyphJustificationInfo.java \
	java/awt/font/GlyphMetrics.java \
	java/awt/font/GlyphVector.java \
	java/awt/font/GraphicAttribute.java \
	java/awt/font/ImageGraphicAttribute.java \
	java/awt/font/LineBreakMeasurer.java \
	java/awt/font/LineMetrics.java \
	java/awt/font/MultipleMaster.java \
	java/awt/font/NumericShaper.java \
	java/awt/font/OpenType.java \
	java/awt/font/ShapeGraphicAttribute.java \
	java/awt/font/TextAttribute.java \
	java/awt/font/TextHitInfo.java \
	java/awt/font/TextLayout.java \
	java/awt/font/TextMeasurer.java \
	java/awt/font/TransformAttribute.java
java_awt_geom_SRCS = \
	java/awt/geom/AffineTransform.java \
	java/awt/geom/Arc2D.java \
	java/awt/geom/Area.java \
	java/awt/geom/CubicCurve2D.java \
	java/awt/geom/Dimension2D.java \
	java/awt/geom/Ellipse2D.java \
	java/awt/geom/FlatteningPathIterator.java \
	java/awt/geom/GeneralPath.java \
	java/awt/geom/IllegalPathStateException.java \
	java/awt/geom/Line2D.java \
	java/awt/geom/NoninvertibleTransformException.java \
	java/awt/geom/PathIterator.java \
	java/awt/geom/Point2D.java \
	java/awt/geom/QuadCurve2D.java \
	java/awt/geom/Rectangle2D.java \
	java/awt/geom/RectangularShape.java \
	java/awt/geom/RoundRectangle2D.java
java_awt_im_SRCS = \
	java/awt/im/InputMethodHighlight.java \
	java/awt/im/InputMethodRequests.java \
	java/awt/im/InputSubset.java
java_awt_image_SRCS = \
	java/awt/image/AreaAveragingScaleFilter.java \
	java/awt/image/BufferedImage.java \
	java/awt/image/BufferedImageOp.java \
	java/awt/image/BufferStrategy.java \
	java/awt/image/ColorModel.java \
	java/awt/image/ComponentColorModel.java \
	java/awt/image/ComponentSampleModel.java \
	java/awt/image/CropImageFilter.java \
	java/awt/image/DataBufferByte.java \
	java/awt/image/DataBufferDouble.java \
	java/awt/image/DataBufferFloat.java \
	java/awt/image/DataBufferInt.java \
	java/awt/image/DataBuffer.java \
	java/awt/image/DataBufferShort.java \
	java/awt/image/DataBufferUShort.java \
	java/awt/image/DirectColorModel.java \
	java/awt/image/FilteredImageSource.java \
	java/awt/image/ImageConsumer.java \
	java/awt/image/ImageFilter.java \
	java/awt/image/ImageObserver.java \
	java/awt/image/ImageProducer.java \
	java/awt/image/ImagingOpException.java \
	java/awt/image/IndexColorModel.java \
	java/awt/image/MemoryImageSource.java \
	java/awt/image/PackedColorModel.java \
	java/awt/image/PixelGrabber.java \
	java/awt/image/PixelInterleavedSampleModel.java \
	java/awt/image/RasterFormatException.java \
	java/awt/image/Raster.java \
	java/awt/image/RasterOp.java \
	java/awt/image/RenderedImage.java \
	java/awt/image/ReplicateScaleFilter.java \
	java/awt/image/RGBImageFilter.java \
	java/awt/image/SampleModel.java \
	java/awt/image/SinglePixelPackedSampleModel.java \
	java/awt/image/TileObserver.java \
	java/awt/image/VolatileImage.java \
	java/awt/image/WritableRaster.java \
	java/awt/image/WritableRenderedImage.java
java_awt_image_renderable_SRCS = \
	java/awt/image/renderable/ContextualRenderedImageFactory.java \
	java/awt/image/renderable/ParameterBlock.java \
	java/awt/image/renderable/RenderableImage.java \
	java/awt/image/renderable/RenderableImageOp.java \
	java/awt/image/renderable/RenderableImageProducer.java \
	java/awt/image/renderable/RenderContext.java \
	java/awt/image/renderable/RenderedImageFactory.java
java_awt_peer_SRCS = \
	java/awt/peer/ButtonPeer.java \
	java/awt/peer/CanvasPeer.java \
	java/awt/peer/CheckboxMenuItemPeer.java \
	java/awt/peer/CheckboxPeer.java \
	java/awt/peer/ChoicePeer.java \
	java/awt/peer/ComponentPeer.java \
	java/awt/peer/ContainerPeer.java \
	java/awt/peer/DialogPeer.java \
	java/awt/peer/FileDialogPeer.java \
	java/awt/peer/FontPeer.java \
	java/awt/peer/FramePeer.java \
	java/awt/peer/LabelPeer.java \
	java/awt/peer/LightweightPeer.java \
	java/awt/peer/ListPeer.java \
	java/awt/peer/MenuBarPeer.java \
	java/awt/peer/MenuComponentPeer.java \
	java/awt/peer/MenuItemPeer.java \
	java/awt/peer/MenuPeer.java \
	java/awt/peer/PanelPeer.java \
	java/awt/peer/PopupMenuPeer.java \
	java/awt/peer/RobotPeer.java \
	java/awt/peer/ScrollbarPeer.java \
	java/awt/peer/ScrollPanePeer.java \
	java/awt/peer/TextAreaPeer.java \
	java/awt/peer/TextComponentPeer.java \
	java/awt/peer/TextFieldPeer.java \
	java/awt/peer/WindowPeer.java
java_awt_print_SRCS = \
	java/awt/print/Book.java \
	java/awt/print/Pageable.java \
	java/awt/print/PageFormat.java \
	java/awt/print/Paper.java \
	java/awt/print/Printable.java \
	java/awt/print/PrinterAbortException.java \
	java/awt/print/PrinterException.java \
	java/awt/print/PrinterGraphics.java \
	java/awt/print/PrinterIOException.java \
	java/awt/print/PrinterJob.java
kaffe_applet_SRCS = \
	kaffe/applet/AppletTag.java \
	kaffe/applet/AppletViewer.java \
	kaffe/applet/AudioPlayer.java
kaffe_awt_SRCS = \
	kaffe/awt/DoNothingPeer.java \
	kaffe/awt/ExportTransferable.java \
	kaffe/awt/FlavorConverter.java \
	kaffe/awt/FocusHook.java \
	kaffe/awt/HookInstaller.java \
	kaffe/awt/ImageClass.java \
	kaffe/awt/ImageDict.java \
	kaffe/awt/ImageSpec.java \
	kaffe/awt/JavaColorModel.java \
	kaffe/awt/KeyHook.java \
	kaffe/awt/MouseHook.java \
	kaffe/awt/ObjectSelection.java \
	kaffe/awt/OpaqueComponent.java \
	kaffe/awt/SerializerConverter.java
kaffe_util_log_SRCS = \
        kaffe/util/log/LogClient.java \
        kaffe/util/log/LogStream.java \
        kaffe/util/log/SimpleLogViewer.java

## Do not delete the previous blank line; update-class-list depends on it

EXTRA_DIST = \
	$(kaffeawt_jar_SRCS)

JAR_CMD1 = (dstdir=`pwd`; cd $(LIBDIR) && $(ZIP) -r $$dstdir/$(CLASSFILE) $(SRCDIRS))

LIBDIR = lib

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

.PHONY: new-classes
new-classes: $(top_srcdir)/developers/update-class-list
	cd $(srcdir) && $(SHELL) $(top_srcdir)/developers/update-class-list

rebuildLib = $(top_builddir)/libraries/javalib/rebuildLib
Klasses_jar = $(top_builddir)/libraries/javalib/rt.jar

.PHONY: compile-classes classes
## classes is provided for backward compatibility; use compile-classes
compile-classes $(LIBDIR)/stamp classes: $(rebuildLib) $(kaffeawt_jar_SRCS)
	rm -rf $(LIBDIR)
	mkdir $(LIBDIR)
	echo $(SHELL) $(rebuildLib)
	CLASSPATH=$(Klasses_jar) $(SHELL) $(rebuildLib) $(kaffeawt_jar_SRCS)
	echo timestamp > $(LIBDIR)/stamp

.PHONY: jar-classes
jar-classes $(CLASSFILE): $(LIBDIR)/stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
