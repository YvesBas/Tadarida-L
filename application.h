#ifndef APPLICATION_H
#define APPLICATION_H


class Application : public QApplication
{
public:
	explicit Application(int& argc, char* argv[]);
	~Application();

};

#endif // APPLICATION_H
