#pragma once

using namespace System;

namespace StillDesign
{
	namespace PhysX
	{
		public interface class IDisposable : System::IDisposable
		{
			public:
				event EventHandler^ OnDisposing;
				event EventHandler^ OnDisposed;
				
				property bool IsDisposed
				{
					bool get();
				}
		};
	};
};