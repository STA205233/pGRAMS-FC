#include "ShutdownSystem.hh"

using namespace anlnext;
namespace gramsballoon {
ANLStatus ShutdownSystem::mod_define() {
  define_parameter("buffer_time_sec", &mod_class::bufferTimeSec_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus ShutdownSystem::mod_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  gettimeofday(&prepareRebootTime_, NULL);
  gettimeofday(&prepareShutdownTime_, NULL);
  gettimeofday(&prepareSoftwareStopTime_, NULL);

  return AS_OK;
}
void ShutdownSystem::clearStatus() {
  setPrepareReboot(false);
  setReboot(false);
  setPrepareShutdown(false);
  setShutdown(false);
  setPrepareSoftwareStop(false);
  setSoftwareStop(false);
  setExitStatus(0);
}

void ShutdownSystem::setPrepareReboot(bool v) {
  singleton_self()->prepareReboot_ = v;
  if (v) {
    gettimeofday(&(singleton_self()->prepareRebootTime_), NULL);
  }
}

void ShutdownSystem::setPrepareShutdown(bool v) {
  singleton_self()->prepareShutdown_ = v;
  if (v) {
    gettimeofday(&(singleton_self()->prepareShutdownTime_), NULL);
  }
}

void ShutdownSystem::setPrepareSoftwareStop(bool v) {
  singleton_self()->prepareSoftwareStop_ = v;
  if (v) {
    gettimeofday(&(singleton_self()->prepareSoftwareStopTime_), NULL);
  }
}

void ShutdownSystem::setReboot(bool v) {
  if (singleton_self()->prepareReboot_) {
    singleton_self()->reboot_ = v;
  }
  else {
    if (getSendTelemetry()) {
      getSendTelemetry()->getErrorManager()->setError(ErrorType::REBOOT_REJECTED);
    }
  }
}

void ShutdownSystem::setShutdown(bool v) {
  if (singleton_self()->prepareShutdown_) {
    singleton_self()->shutdown_ = v;
  }
  else {
    if (getSendTelemetry()) {
      getSendTelemetry()->getErrorManager()->setError(ErrorType::SHUTDOWN_REJECTED);
    }
  }
}

void ShutdownSystem::setSoftwareStop(bool v) {
  if (singleton_self()->prepareSoftwareStop_) {
    singleton_self()->softwareStop_ = v;
  }
  else {
    if (getSendTelemetry()) {
      getSendTelemetry()->getErrorManager()->setError(ErrorType::SOFTWARE_STOP_REJECTED);
    }
  }
}

ANLStatus ShutdownSystem::mod_analyze() {
  if (reboot_) {
    std::cout << "Goes to mod_finalyze" << std::endl;
    return AS_QUIT_ALL;
  }
  else if (shutdown_) {
    std::cout << "Goes to mod_finalyze" << std::endl;
    return AS_QUIT_ALL;
  }
  else if (softwareStop_) {
    std::cout << "Goes to mod_finalyze" << std::endl;
    return AS_QUIT_ALL;
  }

  if (prepareReboot_ && prepareShutdown_) {
    clearStatus();
    return AS_OK;
  }

  if (prepareReboot_ && prepareSoftwareStop_) {
    clearStatus();
    return AS_OK;
  }

  if (prepareShutdown_ && prepareSoftwareStop_) {
    clearStatus();
    return AS_OK;
  }

  if (prepareReboot_) {
    timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec - prepareRebootTime_.tv_sec > bufferTimeSec_) {
      prepareReboot_ = false;
    }
    if (chatter_ >= 1) {
      std::cout << "Prepare Reboot" << std::endl;
    }
  }

  if (prepareShutdown_) {
    timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec - prepareShutdownTime_.tv_sec > bufferTimeSec_) {
      prepareShutdown_ = false;
    }
    if (chatter_ >= 1) {
      std::cout << "Prepare Shutdown" << std::endl;
    }
  }

  if (prepareSoftwareStop_) {
    timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec - prepareSoftwareStopTime_.tv_sec > bufferTimeSec_) {
      prepareSoftwareStop_ = false;
    }
    if (chatter_ >= 1) {
      std::cout << "Prepare Software Stop" << std::endl;
    }
  }

  return AS_OK;
}
ANLStatus ShutdownSystem::mod_finalize() {
  define_result("exit_status", &mod_class::exitStatus_);
  int rslt = 0;
  if (reboot_) {
    std::cout << "Reboot Executed!" << std::endl;
  }
  else if (shutdown_) {
    std::cout << "Shutdown Executed!" << std::endl;
  }
  else if (softwareStop_) {
  }

  if (rslt < 0) {
    std::cerr << "Error in ShutdownSystem::mod_analyze()" << std::endl;
    std::cerr << "Error code: " << strerror(errno) << std::endl;
  }

  return AS_OK;
}
} // namespace gramsballoon