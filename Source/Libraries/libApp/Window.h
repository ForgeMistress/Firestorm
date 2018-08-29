////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Window
//
//  Abstracts the creation of a window that provides a graphics context to draw to.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_WINDOW_H_
#define LIBAPP_WINDOW_H_
#pragma once

#include <libMath/Vector.h>

OPEN_NAMESPACE(Elf);

class Window final
{
public:
	Window();
	~Window();

	/**
		Initialize the window.
	 **/
	bool Initialize(const String& title = "Elflord Application",
                    Vector_2D size = Vector_2D(800, 600));

	/**
		Retrieve the title of the window.
	 **/
	const String& GetTitle() const;

	/**
		Set the title of the window.
	 **/
	void SetTitle(const String& title);

	/**
		Retieve the dimensions of the window.
	 **/
	Vector_2D GetDimensions() const;

	/**
		Set the dimensions of the window.
	 **/
	void SetDimensions(const Vector_2D& vec);

	/**
		Update the window so that it refreshes.
	 **/
	void Update();

	/**
		Close the window.
	 **/
	void Close();

	struct Impl final
	{
		struct ImplData;
		Impl(Window* window);
		~Impl();

		bool Initialize(const String& title, Vector_2D size = Vector_2D(800,600));

		const String& GetTitle() const;
		void SetTitle(const String& title);

		Vector_2D GetDimensions() const;
		void SetDimensions(const Vector_2D& vec);

		void Update();

		void Close();

		Window* window;
		std::unique_ptr<ImplData> data;
	};
private:
	std::unique_ptr<Impl> _impl;
};

CLOSE_NAMESPACE(Elf);

#endif