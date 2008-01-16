CLASSFILE = kaffeawt.jar
jrelib_DATA = $(CLASSFILE)
JAR_CMD1 = $(FASTJAR) cf $(CLASSFILE) `find . -name "*.class"`

CPATH =  $(GLIBJ_ZIP):$(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar
AM_JAVACFLAGS = -bootclasspath $(CPATH)

$(CLASSFILE): classdist_kaffeawt.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)

clean-local:
	rm -rf $(CLASSFILE) `find . -name "*.class"`

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
kaffeawtdir = $(FAKE)

dist_kaffeawt_JAVA = \
	java/awt/AWTEvent.java \
	java/awt/ActionEvt.java \
	java/awt/AdjustmentEvt.java \
	java/awt/BarMenu.java \
	java/awt/Button.java \
	java/awt/Canvas.java \
	java/awt/Checkbox.java \
	java/awt/CheckboxGroup.java \
	java/awt/CheckboxMenuItem.java \
	java/awt/Choice.java \
	java/awt/ClassAnalyzer.java \
	java/awt/ClassProperties.java \
	java/awt/Component.java \
	java/awt/ComponentEvt.java \
	java/awt/Container.java \
	java/awt/ContainerEvt.java \
	java/awt/Cursor.java \
	java/awt/DefKeyFilter.java \
	java/awt/Defaults.java \
	java/awt/Dialog.java \
	java/awt/Dimension.java \
	java/awt/Event.java \
	java/awt/EventDispatchThread.java \
	java/awt/EventQueue.java \
	java/awt/FileDialog.java \
	java/awt/FlowLayout.java \
	java/awt/FocusEvt.java \
	java/awt/Font.java \
	java/awt/FontMetrics.java \
	java/awt/Frame.java \
	java/awt/Graphics.java \
	java/awt/GraphicsLink.java \
	java/awt/Image.java \
	java/awt/ImageFrameLoader.java \
	java/awt/ImageLoader.java \
	java/awt/ImageNativeProducer.java \
	java/awt/Insets.java \
	java/awt/ItemEvt.java \
	java/awt/KeyEvt.java \
	java/awt/Label.java \
	java/awt/List.java \
	java/awt/MediaTracker.java \
	java/awt/Menu.java \
	java/awt/MenuBar.java \
	java/awt/MenuComponent.java \
	java/awt/MenuItem.java \
	java/awt/MenuShortcut.java \
	java/awt/MouseEvt.java \
	java/awt/NativeClipboard.java \
	java/awt/NativeGraphics.java \
	java/awt/NativeSelection.java \
	java/awt/OpaqueComponent.java \
	java/awt/PSGraphics.java \
	java/awt/PSPrintJob.java \
	java/awt/PaintEvt.java \
	java/awt/Panel.java \
	java/awt/PopupMenu.java \
	java/awt/PopupWindow.java \
	java/awt/PrintJob.java \
	java/awt/RootWindow.java \
	java/awt/RowCanvas.java \
	java/awt/ScrollPane.java \
	java/awt/Scrollbar.java \
	java/awt/ShortcutConsumer.java \
	java/awt/ShortcutHandler.java \
	java/awt/SystemColor.java \
	java/awt/TextArea.java \
	java/awt/TextBuffer.java \
	java/awt/TextComponent.java \
	java/awt/TextCursor.java \
	java/awt/TextEvt.java \
	java/awt/TextField.java \
	java/awt/Toolkit.java \
	java/awt/WMEvent.java \
	java/awt/Window.java \
	java/awt/WindowEvt.java \
	java/awt/event/AdjustmentEvent.java \
	java/awt/event/ComponentAdapter.java \
	java/awt/event/ComponentEvent.java \
	java/awt/event/ContainerAdapter.java \
	java/awt/event/ContainerEvent.java \
	java/awt/event/FocusAdapter.java \
	java/awt/event/InputEvent.java \
	java/awt/event/ItemEvent.java \
	java/awt/event/KeyAdapter.java \
	java/awt/event/KeyEvent.java \
	java/awt/event/MouseAdapter.java \
	java/awt/event/MouseEvent.java \
	java/awt/event/MouseMotionAdapter.java \
	java/awt/event/PaintEvent.java \
	java/awt/event/TextEvent.java \
	java/awt/event/WindowAdapter.java \
	java/awt/event/WindowEvent.java \
	java/awt/image/AreaAveragingScaleFilter.java \
	java/awt/image/CropImageFilter.java \
	java/awt/image/FilteredImageSource.java \
	java/awt/image/ImageConsumer.java \
	java/awt/image/ImageFilter.java \
	java/awt/image/ImageObserver.java \
	java/awt/image/ImageProducer.java \
	java/awt/image/IndexColorModel.java \
	java/awt/image/MemoryImageSource.java \
	java/awt/image/PixelGrabber.java \
	java/awt/image/RGBImageFilter.java \
	java/awt/image/ReplicateScaleFilter.java \
	org/kaffe/awt/DoNothingPeer.java \
	org/kaffe/awt/ExportTransferable.java \
	org/kaffe/awt/FlavorConverter.java \
	org/kaffe/awt/FocusHook.java \
	org/kaffe/awt/HookInstaller.java \
	org/kaffe/awt/ImageClass.java \
	org/kaffe/awt/ImageDict.java \
	org/kaffe/awt/ImageSpec.java \
	org/kaffe/awt/JavaColorModel.java \
	org/kaffe/awt/KeyHook.java \
	org/kaffe/awt/MouseHook.java \
	org/kaffe/awt/ObjectSelection.java \
	org/kaffe/awt/OpaqueComponent.java \
	org/kaffe/awt/SerializerConverter.java \
	org/kaffe/io/AccessibleBAOStream.java \
	org/kaffe/util/log/LogClient.java \
	org/kaffe/util/log/LogStream.java \
	org/kaffe/util/log/SimpleLogViewer.java \
	org/kaffe/util/DoubleLinkedObject.java \
	org/kaffe/util/Timer.java \
	org/kaffe/util/TimerClient.java \
	org/kaffe/util/VectorSnapshot.java 

EXTRA_DIST = \
	java/awt/win32/ActionEvt.java \
	java/awt/win32/Button.java \
	java/awt/win32/Canvas.java \
	java/awt/win32/Checkbox.java \
	java/awt/win32/CheckboxMenuItem.java \
	java/awt/win32/Choice.java \
	java/awt/win32/Defaults.java \
	java/awt/win32/FileDialog.java \
	java/awt/win32/Frame.java \
	java/awt/win32/Label.java \
	java/awt/win32/List.java \
	java/awt/win32/Menu.java \
	java/awt/win32/MenuBar.java \
	java/awt/win32/MenuItem.java \
	java/awt/win32/NativeComponent.java \
	java/awt/win32/NativeContainer.java \
	java/awt/win32/NativeGraphics.java \
	java/awt/win32/Panel.java \
	java/awt/win32/PopupMenu.java \
	java/awt/win32/ScrollPane.java \
	java/awt/win32/Scrollbar.java \
	java/awt/win32/SystemColor.java \
	java/awt/win32/TextArea.java \
	java/awt/win32/TextComponent.java \
	java/awt/win32/TextField.java \
	java/awt/win32/Toolkit.java \
	java/awt/win32/WMEvent.java \
	java/awt/win32/Window.java 

