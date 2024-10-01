from conans import ConanFile

class PBE(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    version = "2.2.3"
    license = "BSD"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/PBE"
    description = "NHERI SimCenter Performance-Based Engineering Tool"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake"
    requires = ["jansson/2.13.1",
               "libcurl/7.72.0",
               "zlib/1.2.11",
               #'quazip/1.4',
               #"hdf5/1.12.0"
               ]

    build_policy = "missing"


    def configure(self):
        
        self.output.success("CONFIGURE")
        
        if self.settings.os == "Windows":
            self.options["lapack"].visual_studio = True
            self.options["lapack"].shared = True

    def imports(self):
        
        self.output.success("IMPORTS")
        
        if self.settings.os == "Windows":
            output = './%s' % self.settings.build_type
            self.copy("lib*.dll", dst=output, src="bin")

        appsDestination = "./applications"
        if self.settings.os == "Windows":
            appsDestination = './%s/applications' % self.settings.build_type
        elif self.settings.os == "Macos":
            appsDestination = './PBE.app/Contents/MacOS'


        appsSRC = "../../SimCenterBackendApplications/applications"
        appsSRC = "bin"        
        
        self.output.success("CONFIGURE %s, %s" % (appsDestination, appsSRC))
                                            
        self.copy("createEVENT/multiple*", dst=appsDestination, src=appsSRC)
        self.copy("createEVENT/siteRes*", dst=appsDestination, src=appsSRC)
        self.copy("createEVENT/stochasticGr*", dst=appsDestination, src=appsSRC)

        self.copy("createSAM/*", dst=appsDestination, src=appsSRC)
        self.copy("createEDP/*", dst=appsDestination, src=appsSRC)
        self.copy("performSIMULATION/*", dst=appsDestination, src=appsSRC)
        self.copy("performUQ/*", dst=appsDestination, src=appsSRC)
        self.copy("performDL/*", dst=appsDestination, src=appsSRC)
        self.copy("Workflow/*", dst=appsDestination, src=appsSRC)
        self.copy("*", dst="{}/opensees".format(appsDestination), src="opensees")
        self.copy("*", dst="{}/dakota".format(appsDestination), src="dakota")
