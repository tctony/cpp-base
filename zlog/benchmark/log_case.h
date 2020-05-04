
class LogCase {
 public:
  virtual const char* name() = 0;
  virtual void run() = 0;
};

LogCase* GetLogCase();