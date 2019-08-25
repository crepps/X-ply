# X-ply
X-ply is an application that takes XBOX controller input and simulates Windows mouse and keyboard events to control specific applications. Ever plugged your computer into a flatscreen to watch a movie? Pressing <code>back + Y</code> on a controller will activate <i>VLC Mode</i>. If you have a Windows-compatable, wireless XBOX controller, you can have a TV remote that works with your computer (i.e. press <code>A</code> to play/pause, <code>D-pad UP</code>/<code>D-pad DOWN</code> to adjust volume, etc).

X-ply has multiple <i>modes</i> (FSM states); for example, <code>back + X</code> will enable <i>WMP Mode</i>, allowing you to control Windows Media Player, and <code>back + A</code> will enable <i>Windows Mode</i>, allowing you to operate Windows in every way you could with a mouse and keyboard - from clicks and alt-tabbing to typing full sentences.

## Example
Here's a snippet from the <code>Controller</code> class's <code>Update</code> function, showing what toggling full screen in <i>VLC Mode</i> looks like. The definition of the <code>KeyEvent</code> function it uses is shown underneath.

```cpp
void Controller::Update()
{
	// ...
  
  	// Toggle full screen
	else if (ButtonPress(Y) && !_processed[Map(Y)])
	{
		KeyEvent('F', 2);
		
		_processed[Map(Y)] = true;
	}
        
	// ...
}

void Controller::KeyEvent(DWORD key, int type)
{
	// Full keystroke
	if (type == 2)
	{
		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0);
		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	}

	// Set key down or up
	else
		keybd_event(static_cast<BYTE>(key), MapVirtualKey(key, MAPVK_VK_TO_VSC), (type ? 0 : KEYEVENTF_KEYUP), 0);
}
```

## Dependencies
This project uses the XInput library, so link <code>Xinput.lib</code> (part of the Windows SDK).
