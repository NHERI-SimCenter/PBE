from conans import ConanFile

class PBE(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    version = "2.1.0"
    license = "BSD"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/PBE"
    description = "NHERI SimCenter Performance-Based Engineering Tool"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake"
    requires = "jansson/2.11@bincrafters/stable", \
               "libcurl/7.64.1@bincrafters/stable", \
               "lapack/3.7.1@conan/stable", \
               "hdf5/1.12.0",\
               "zlib/1.2.11@conan/stable",\
               "SimCenterCommonQt/0.1.9@simcenter/stable",\
               "SimCenterBackendApplications/1.2.2@simcenter/stable",\
               "opensees/3.2.2@simcenter/testing",\
               "dakota/6.12.0@simcenter/testing"

    build_policy = "missing"


    def configure(self):
        if self.settings.os == "Windows":
            self.options["libcurl"].with_winssl = True
            self.options["libcurl"].with_openssl = False
            self.options["lapack"].visual_studio = True
            self.options["lapack"].shared = True

    def imports(self):
        if self.settings.os == "Windows":
            output = './%s' % self.settings.build_type
            self.copy("lib*.dll", dst=output, src="bin")

        appsDestination = "./applications"
        if self.settings.os == "Windows":
            appsDestination = './%s/applications' % self.settings.build_type
        elif self.settings.os == "Macos":
            appsDestination = './PBE.app/Contents/MacOS/applications'

        self.copy("createEVENT/multiple*", dst=appsDestination, src="bin")
        self.copy("createEVENT/siteRes*", dst=appsDestination, src="bin")
        self.copy("createEVENT/stochasticGr*", dst=appsDestination, src="bin")

        self.copy("createSAM/*", dst=appsDestination, src="bin")
        self.copy("createEDP/*", dst=appsDestination, src="bin")
        self.copy("performSIMULATION/*", dst=appsDestination, src="bin")
        self.copy("performUQ/*", dst=appsDestination, src="bin")
        self.copy("performDL/*", dst=appsDestination, src="bin")
        self.copy("Workflow/*", dst=appsDestination, src="bin")
        self.copy("*", dst="{}/opensees".format(appsDestination), src="opensees")
        self.copy("*", dst="{}/dakota".format(appsDestination), src="dakota")
