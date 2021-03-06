/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * OpenCL rendering engine
 */

#ifndef COPENCLRENDERER_H
#define COPENCLRENDERER_H

#include "CBaseRenderer.h"
#include "ocl.h"
#include "COpticalField.h"

#include <vector>
#include <string>


/**
 * A class to render holograms using OpenCL
 */
class COpenCLRenderer : public CBaseRenderer
{
  public:
    enum EAlgorithmType {
      ALGORITHM_SINGLEPASS,          /// single pass algorithm
      ALGORITHM_MULTIPASS,           /// multipass rendering algorithm (the work load is split into chunks which are sent to device individually if necessary)
      ALGORITHM_MULTIPASS_CPU,       /// CPU based multipass algorithm
      ALGORITHM_MULTIPASS_NATIVE,    /// GPU based multipass algorithm that uses native instructions
      ALGORITHM_MULTIPASS_ALIGNED,   /// GPU based multipass algorithm that requires aligned point cloud memory
      ALGORITHM_INVALID              /// an invalid algorithm type marker
    };

  public:
    /**
     * Default constructor
     */
    explicit COpenCLRenderer(double hologram_z = 0)
      : CBaseRenderer(hologram_z),
        m_context(),
        m_cmd_queue(),
        m_program(),
        m_kernel(),
        m_device(),
        m_chunk_size(0),
        m_mem_obj_max_size(0),
        m_alg_type(),
        m_err_msg("")
    {
    }

    /**
     * Destructor
     */
    virtual ~COpenCLRenderer(void)
    {
      close();
    }

    /**
     * A method to set the type of rendering algorithm
     */
    void setAlgorithmType(EAlgorithmType type)
    {
      m_alg_type = type;
      return;
    }

    /**
     * A method to retrieve the value of currently used
     * chunk size (in bytes)
     */
    cl_ulong getChunkSize(void) const
    {
      return m_chunk_size;
    }

    /**
     * A method to set the preferred chunk size (in optical field elements)
     * If this value is not set then the open() method
     * will set the appropriate value according to
     * the maximum number of bytes that can be allocated
     * for a single memory object.
     */
    void setChunkSize(size_t chunk_size)
    {
      m_chunk_size = chunk_size;
      return;
    }

    /* implement the IRenderer interface */
    virtual bool open(const char *filename = NULL);
    virtual bool close(void);
    virtual std::string getError(void) const;
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of);
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of);


    /**
     * A function to convert algorithm type to a string description.
     */
    static const char *algToStr(EAlgorithmType type);

    
  private:
    /* disable assignment, copy and move
       If I wanted to implement these someday in future
       I would have to do so in terms of retain and release */
    COpenCLRenderer(const COpenCLRenderer & );
    HOLOREN_DECL_MOVE_CONSTRUCTOR(COpenCLRenderer);
    COpenCLRenderer & operator=(const COpenCLRenderer & );

  private:
    /**
     * This rendering algorithm does only one rendering pass, i.e. it enqueues kernel only once
     * and thus it must entirely fit into GPU-s memory
     */
    bool renderAlgorithm_SinglePass(const CPointCloud & pc, cl_mem pc_buf, COpticalField *of, cl_mem of_buf);
    
    /**
     * The fourth rendering algorithm, can render large holograms, by dividing the calculation
     * in several smaller chunks
     */
    bool renderAlgorithm_MultiPass(const CPointCloud & pc, cl_mem pc_buf, COpticalField *of, cl_mem of_buf);

    /**
     * A function to fill the contents of point cloud memory object
     */
    bool fillPCMemObj(const CPointCloud & pc, cl_mem pc_buf);
    
    /**
     * A method to read the opencl program from a file
     *
     * On error values of program_buf and program_buf_size are undefined
     */
    bool readCLSource(const char *filename, std::string *program_buf);

    /**
     * A function to create a build log for the device
     */
    void constructBuildLog(cl_int err);

  private:
    /** An enumeration of error codes */
    enum EErrorCode {
      ERR_OK = 0,     /// no error
      ERR_OPEN,       /// error opening OpenCL source file
      ERR_EMPTY,      /// the file with OpenCL program was empty
      ERR_READ,       /// an error reading OpenCL source file
      ERR_BUILD,      /// an error building OpenCL source
      ERR_UNKNOWN     /// an undefined error
    };

  private:
    cl_context m_context;          /// OpenCL context (needed for buffer creation and cannot be destroyd before all computations are done)
    cl_command_queue m_cmd_queue;  /// OpenCL command queue (needed to enqueue commands to devices - reads, writes, start of computation, ...)
    cl_program m_program;          /// OpenCL program (destroying it before the computations are done, won't probably do any good)
    cl_kernel m_kernel;            /// kernel that will be executed (will be sent to device for execution)
    cl_device_id m_device;         /// the device used
    size_t m_chunk_size;           /// determines the amount of data that can be allocated and processed at once by the kernel
    cl_ulong m_mem_obj_max_size;   /// describes the maximum size of a memory object on a given device (in bytes)
    EAlgorithmType m_alg_type;     /// the type of algorithm to be used
    std::string m_err_msg;         /// a string to hold the error message and a build log
};

#endif
